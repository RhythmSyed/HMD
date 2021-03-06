/*
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Defender internal include. */
#include "private/aws_iot_defender_internal.h"

#include "aws_secure_sockets.h"

#include "iot_metrics.h"

#include "platform/aws_iot_clock.h"

#define  _HEADER_TAG           AwsIotDefenderInternal_SelectTag( "header", "hed" )
#define  _REPORTID_TAG         AwsIotDefenderInternal_SelectTag( "report_id", "rid" )
#define  _VERSION_TAG          AwsIotDefenderInternal_SelectTag( "version", "v" )
#define  _VERSION_1_0          "1.0" /* Used by defender service to indicate the schema change of report, e.g. adding new field. */
#define  _METRICS_TAG          AwsIotDefenderInternal_SelectTag( "metrics", "met" )

/* 15 for IP + 1 for ":" + 5 for port + 1 terminator
 * For example: "192.168.0.1:8000\0"
 */
#define _REMOTE_ADDR_LENGTH    22

#define _TCP_CONN_TAG          AwsIotDefenderInternal_SelectTag( "tcp_connections", "tc" )
#define _EST_CONN_TAG          AwsIotDefenderInternal_SelectTag( "established_connections", "ec" )
#define _TOTAL_TAG             AwsIotDefenderInternal_SelectTag( "total", "t" )
#define _CONN_TAG              AwsIotDefenderInternal_SelectTag( "connections", "cs" )
#define _REMOTE_ADDR_TAG       AwsIotDefenderInternal_SelectTag( "remote_addr", "rad" )

/**
 * Structure to hold a metrics report.
 */
typedef struct _metricsReport
{
    AwsIotSerializerEncoderObject_t object; /* Encoder object handle. */
    uint8_t * pDataBuffer;                  /* Raw data buffer to be published with MQTT. */
    size_t size;                            /* Raw data size. */
} _metricsReport_t;

typedef struct _tcpConns
{
    IotMetricsTcpConnection_t * pArray;
    uint8_t count;
} _tcpConns_t;

typedef struct _metrics
{
    _tcpConns_t tcpConns;
} _metrics_t;

/* Initialize metrics report. */
static _metricsReport_t _report =
{
    .object      = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM,
    .pDataBuffer = NULL,
    .size        = 0
};

/* Initialize metrics data. */
static _metrics_t _metrics = { 0 };

/* Define a "snapshot" global array of metrics flag. */
static uint32_t _metricsFlagSnapshot[ _DEFENDER_METRICS_GROUP_COUNT ];

static void copyMetricsFlag();

static AwsIotDefenderEventType_t getLatestMetricsData();

static void freeMetricsData();

static void tcpConnectionsCallback( void * param1,
                                    IotListDouble_t * pTcpConnectionsMetricsList );

static void _serialize();

static void _serializeTcpConnections( AwsIotSerializerEncoderObject_t * pMetricsObject );

/**
 * Attempt to serialize metrics report with given data buffer.
 */

/*static bool _serialize( AwsIotSerializerEncoderObject_t * pEncoderObject,
 *                      uint8_t * pDataBuffer,
 *                      size_t dataSize );*/

/*-----------------------------------------------------------*/

void assertSuccess( AwsIotSerializerError_t error )
{
    AwsIotDefender_Assert( error == AWS_IOT_SERIALIZER_SUCCESS );
}

/*-----------------------------------------------------------*/

void assertSuccessOrBufferToSmall( AwsIotSerializerError_t error )
{
    AwsIotDefender_Assert( error == AWS_IOT_SERIALIZER_SUCCESS || error == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL );
}

/*-----------------------------------------------------------*/

uint8_t * AwsIotDefenderInternal_GetReportBuffer()
{
    return _report.pDataBuffer;
}

/*-----------------------------------------------------------*/

size_t AwsIotDefenderInternal_GetReportBufferSize()
{
    /* Encoder might over-calculate the needed size. Therefor encoded size might be smaller than buffer size: _report.size. */
    return _AwsIotDefenderEncoder.getEncodedSize( &_report.object, _report.pDataBuffer );
}

/*-----------------------------------------------------------*/

AwsIotDefenderEventType_t AwsIotDefenderInternal_CreateReport()
{
    /* Assert report buffer is not allocated. */
    AwsIotDefender_Assert( _report.pDataBuffer == NULL && _report.size == 0 );

    AwsIotDefenderEventType_t eventError = 0;

    AwsIotSerializerEncoderObject_t * pEncoderObject = &( _report.object );
    size_t dataSize = 0;
    uint8_t * pReportBuffer = NULL;

    /* Copy the metrics flag user specified. */
    copyMetricsFlag();

    /* Get latest metrics data. */
    eventError = getLatestMetricsData();

    if( !eventError )
    {
        /* Dry-run serialization to calculate the required size. */
        _serialize();

        /* Get the calculated required size. */
        dataSize = _AwsIotDefenderEncoder.getExtraBufferSizeNeeded( pEncoderObject );

        /* Clean the encoder object handle. */
        _AwsIotDefenderEncoder.destroy( pEncoderObject );

        /* Allocate memory once. */
        pReportBuffer = AwsIotDefender_MallocReport( dataSize * sizeof( uint8_t ) );

        if( pReportBuffer != NULL )
        {
            _report.pDataBuffer = pReportBuffer;
            _report.size = dataSize;

            /* Actual serialization. */
            _serialize();
        }
        else
        {
            eventError = AWS_IOT_DEFENDER_EVENT_NO_MEMORY;
        }

        /* Metrics data can be freed. */
        freeMetricsData();
    }

    return eventError;
}

/*-----------------------------------------------------------*/

void AwsIotDefenderInternal_DeleteReport()
{
    /* Destroy the encoder object. */
    _AwsIotDefenderEncoder.destroy( &( _report.object ) );

    /* Free the memory of data buffer. */
    AwsIotDefender_FreeReport( _report.pDataBuffer );

    /* Reset report members. */
    _report.pDataBuffer = NULL;
    _report.size = 0;
    _report.object = ( AwsIotSerializerEncoderObject_t ) AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_STREAM;

    _metrics = ( _metrics_t ) {
        0
    };
}

/*
 * report:
 * {
 *  "header": {
 *      "report_id": 1530304554,
 *      "version": "1.0"
 *  },
 *  "metrics": {
 *      ...
 *  }
 * }
 */
static void _serialize()
{
    AwsIotSerializerError_t serializerError = AWS_IOT_SERIALIZER_SUCCESS;

    AwsIotSerializerEncoderObject_t * pEncoderObject = &( _report.object );

    AwsIotSerializerEncoderObject_t reportMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t headerMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t metricsMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

    /* Define an assert function for serialization returned error. */
    void (* assertNoError)( AwsIotSerializerError_t ) = _report.pDataBuffer == NULL ? assertSuccessOrBufferToSmall
                                                        : assertSuccess;

    uint8_t metricsGroupCount = 0;

    serializerError = _AwsIotDefenderEncoder.init( pEncoderObject, _report.pDataBuffer, _report.size );
    assertNoError( serializerError );

    /* Create the outermost map with 2 keys: "header", "metrics". */
    serializerError = _AwsIotDefenderEncoder.openContainer( pEncoderObject, &reportMap, 2 );
    assertNoError( serializerError );

    /* Create the "header" map with 2 keys: "report_id", "version". */
    serializerError = _AwsIotDefenderEncoder.openContainerWithKey( &reportMap,
                                                                   _HEADER_TAG,
                                                                   &headerMap,
                                                                   2 );
    assertNoError( serializerError );

    /* Append key-value pair of "report_Id" which uses clock time. */
    serializerError = _AwsIotDefenderEncoder.appendKeyValue( &headerMap,
                                                             _REPORTID_TAG,
                                                             AwsIotSerializer_ScalarSignedInt( AwsIotClock_GetTimeMs() ) );
    assertNoError( serializerError );

    /* Append key-value pair of "version". */
    serializerError = _AwsIotDefenderEncoder.appendKeyValue( &headerMap,
                                                             _VERSION_TAG,
                                                             AwsIotSerializer_ScalarTextString( _VERSION_1_0 ) );
    assertNoError( serializerError );

    /* Close the "header" map. */
    serializerError = _AwsIotDefenderEncoder.closeContainer( &reportMap, &headerMap );
    assertNoError( serializerError );

    /* Count how many metrics groups user specified. */
    for( uint8_t i = 0; i < _DEFENDER_METRICS_GROUP_COUNT; i++ )
    {
        metricsGroupCount += _metricsFlagSnapshot[ i ] > 0;
    }

    /* Create the "metrics" map with number of keys as the number of metrics groups. */
    serializerError = _AwsIotDefenderEncoder.openContainerWithKey( &reportMap,
                                                                   _METRICS_TAG,
                                                                   &metricsMap,
                                                                   metricsGroupCount );
    assertNoError( serializerError );

    for( uint8_t i = 0; i < _DEFENDER_METRICS_GROUP_COUNT; i++ )
    {
        /* Skip if this metrics group has 0 metrics flag. */
        if( _metricsFlagSnapshot[ i ] )
        {
            switch( i )
            {
                case AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS:
                    _serializeTcpConnections( &metricsMap );
                    break;

                default:
                    /* The index of metricsFlagSnapshot must be one of the metrics group. */
                    AwsIotDefender_Assert( 0 );
            }
        }
    }

    /* Close the "metrics" map. */
    serializerError = _AwsIotDefenderEncoder.closeContainer( &reportMap, &metricsMap );
    assertNoError( serializerError );

    /* Close the "report" map. */
    serializerError = _AwsIotDefenderEncoder.closeContainer( pEncoderObject, &reportMap );
    assertNoError( serializerError );
}

/*-----------------------------------------------------------*/

static void copyMetricsFlag()
{
    /* Copy the metrics flags to snapshot so that it is unlocked quicker. */
    AwsIotMutex_Lock( &_AwsIotDefenderMetrics.mutex );

    /* Memory copy from the metricsFlag array to metricsFlagSnapshot array. */
    memcpy( _metricsFlagSnapshot, _AwsIotDefenderMetrics.metricsFlag, sizeof( _metricsFlagSnapshot ) );

    AwsIotMutex_Unlock( &_AwsIotDefenderMetrics.mutex );
}

/*-----------------------------------------------------------*/

static AwsIotDefenderEventType_t getLatestMetricsData()
{
    AwsIotDefenderEventType_t eventError = 0;

    /* Get TCP connections metrics data. */
    if( _metricsFlagSnapshot[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] )
    {
        IotMetricsListCallback_t tcpConnectionscallback;


        tcpConnectionscallback.function = tcpConnectionsCallback;
        tcpConnectionscallback.param1 = ( void * ) &eventError;

        IotMetrics_ProcessTcpConnections( tcpConnectionscallback );
    }

    return eventError;
}

/*-----------------------------------------------------------*/

static void freeMetricsData()
{
    if( _metricsFlagSnapshot[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ] )
    {
        AwsIotMetrics_FreeTcpConnection( _metrics.tcpConns.pArray );
        _metrics.tcpConns.pArray = NULL;
        _metrics.tcpConns.count = 0;
    }
}

/*-----------------------------------------------------------*/

static void tcpConnectionsCallback( void * param1,
                                    IotListDouble_t * pTcpConnectionsMetricsList )
{
    AwsIotDefenderEventType_t * pEventError = ( AwsIotDefenderEventType_t * ) param1;

    IotLink_t * pConnectionLink = IotListDouble_PeekHead( pTcpConnectionsMetricsList );
    IotMetricsTcpConnection_t * pConnection = NULL;

    size_t total = IotListDouble_Count( pTcpConnectionsMetricsList );

    /* If there is at least one TCP connection. */
    if( total > 0 )
    {
        /* Allocate memory to copy TCP connections metrics data. */
        _metrics.tcpConns.pArray = AwsIotMetrics_MallocTcpConnection( total * sizeof( IotMetricsTcpConnection_t ) );

        if( _metrics.tcpConns.pArray != NULL )
        {
            /* Set count only the memory allocation succeeds. */
            _metrics.tcpConns.count = ( uint8_t ) total;

            /* At least one element in the list*/
            AwsIotDefender_Assert( pConnectionLink );

            for( uint8_t i = 0; i < total; i++ )
            {
                pConnection = IotLink_Container( IotMetricsTcpConnection_t, pConnectionLink, link );

                /* Copy to new allocated array. */
                _metrics.tcpConns.pArray[ i ] = *pConnection;

                /* Iterate to next one. */
                pConnectionLink = pConnectionLink->pNext;
            }
        }
        else
        {
            *pEventError = AWS_IOT_DEFENDER_EVENT_NO_MEMORY;
        }
    }
}

/*-----------------------------------------------------------*/

static void _serializeTcpConnections( AwsIotSerializerEncoderObject_t * pMetricsObject )
{
    AwsIotDefender_Assert( pMetricsObject != NULL );

    AwsIotSerializerError_t serializerError = AWS_IOT_SERIALIZER_SUCCESS;

    AwsIotSerializerEncoderObject_t tcpConnectionMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t establishedMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;
    AwsIotSerializerEncoderObject_t connectionsArray = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_ARRAY;

    uint32_t tcpConnFlag = _AwsIotDefenderMetrics.metricsFlag[ AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS ];

    uint8_t hasEstablishedConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED ) > 0;
    /* Whether "connections" should show up is not only determined by user input, but also if there is at least 1 connection. */
    uint8_t hasConnections = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_CONNECTIONS ) > 0 &&
                             ( _metrics.tcpConns.count > 0 );
    uint8_t hasTotal = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_TOTAL ) > 0;
    uint8_t hasRemoteAddr = ( tcpConnFlag & AWS_IOT_DEFENDER_METRICS_TCP_CONNECTIONS_ESTABLISHED_REMOTE_ADDR ) > 0;

    char remoteAddr[ _REMOTE_ADDR_LENGTH ] = "";
    char * pRemoteAddr = remoteAddr;

    void (* assertNoError)( AwsIotSerializerError_t ) = _report.pDataBuffer == NULL ? assertSuccessOrBufferToSmall
                                                        : assertSuccess;

    /* Create the "tcp_connections" map with 1 key "established_connections" */
    serializerError = _AwsIotDefenderEncoder.openContainerWithKey( pMetricsObject,
                                                                   _TCP_CONN_TAG,
                                                                   &tcpConnectionMap,
                                                                   1 );
    assertNoError( serializerError );

    /* if user specify any metrics under "established_connections" */
    if( hasEstablishedConnections )
    {
        /* Create the "established_connections" map with "total" and/or "connections". */
        serializerError = _AwsIotDefenderEncoder.openContainerWithKey( &tcpConnectionMap,
                                                                       _EST_CONN_TAG,
                                                                       &establishedMap,
                                                                       hasConnections + hasTotal );
        assertNoError( serializerError );

        /* if user specify any metrics under "connections" and there are at least one connection */
        if( hasConnections )
        {
            AwsIotDefender_Assert( _metrics.tcpConns.pArray != NULL );

            /* create array "connections" under "established_connections" */
            serializerError = _AwsIotDefenderEncoder.openContainerWithKey( &establishedMap,
                                                                           _CONN_TAG,
                                                                           &connectionsArray,
                                                                           _metrics.tcpConns.count );
            assertNoError( serializerError );

            for( uint8_t i = 0; i < _metrics.tcpConns.count; i++ )
            {
                AwsIotSerializerEncoderObject_t connectionMap = AWS_IOT_SERIALIZER_ENCODER_CONTAINER_INITIALIZER_MAP;

                /* open a map under "connections" */
                serializerError = _AwsIotDefenderEncoder.openContainer( &connectionsArray,
                                                                        &connectionMap,
                                                                        hasRemoteAddr );
                assertNoError( serializerError );

                /* add remote address */
                if( hasRemoteAddr )
                {
                    /* Remote IP is with host endian. So it is converted to network endian and passed into SOCKETS_inet_ntoa. */
                    SOCKETS_inet_ntoa( SOCKETS_htonl( _metrics.tcpConns.pArray[ i ].remoteIP ), remoteAddr );
                    sprintf( remoteAddr, "%s:%d", remoteAddr, _metrics.tcpConns.pArray[ i ].remotePort );

                    serializerError = _AwsIotDefenderEncoder.appendKeyValue( &connectionMap, _REMOTE_ADDR_TAG,
                                                                             AwsIotSerializer_ScalarTextString( pRemoteAddr ) );
                    assertNoError( serializerError );
                }

                serializerError = _AwsIotDefenderEncoder.closeContainer( &connectionsArray, &connectionMap );
                assertNoError( serializerError );
            }

            serializerError = _AwsIotDefenderEncoder.closeContainer( &establishedMap, &connectionsArray );
            assertNoError( serializerError );
        }

        if( hasTotal )
        {
            serializerError = _AwsIotDefenderEncoder.appendKeyValue( &establishedMap,
                                                                     _TOTAL_TAG,
                                                                     AwsIotSerializer_ScalarSignedInt( _metrics.tcpConns.count ) );
            assertNoError( serializerError );
        }

        serializerError = _AwsIotDefenderEncoder.closeContainer( &tcpConnectionMap, &establishedMap );
        assertNoError( serializerError );
    }

    serializerError = _AwsIotDefenderEncoder.closeContainer( pMetricsObject, &tcpConnectionMap );
    assertNoError( serializerError );
}

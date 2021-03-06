/*
 * Amazon FreeRTOS System Initialization
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_iot_serializer_tinycbor_encoder.c
 * @brief Implements APIs to serialize data in CBOR format. The file relies on tiny
 * CBOR library to serialize data into CBOR format. Supports all major data types within
 * the CBOR format.
 * The file implements the encoder interface in aws_iot_serialize.h.
 */

#include "aws_iot_serializer.h"
#include "cbor.h"

/* Translate cbor error to serializer error. */
static void _translateErrorCode( CborError cborError,
                                 AwsIotSerializerError_t * pSerializerError );

static size_t _getEncodedSize( AwsIotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer );
static size_t _getExtraBufferSizeNeeded( AwsIotSerializerEncoderObject_t * pEncoderObject );
static AwsIotSerializerError_t _init( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                      uint8_t * pDataBuffer,
                                      size_t maxSize );
static void _destroy( AwsIotSerializerEncoderObject_t * pEncoderObject );
static AwsIotSerializerError_t _openContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                               AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                               size_t length );

static AwsIotSerializerError_t _openContainerWithKey( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                      const char * pKey,
                                                      AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                      size_t length );
static AwsIotSerializerError_t _closeContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                AwsIotSerializerEncoderObject_t * pNewEncoderObject );
static AwsIotSerializerError_t _append( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        AwsIotSerializerScalarData_t scalarData );
static AwsIotSerializerError_t _appendKeyValue( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                const char * pKey,
                                                AwsIotSerializerScalarData_t scalarData );


AwsIotSerializerEncodeInterface_t _AwsIotSerializerCborEncoder =
{
    .getEncodedSize           = _getEncodedSize,
    .getExtraBufferSizeNeeded = _getExtraBufferSizeNeeded,
    .init                     = _init,
    .destroy                  = _destroy,
    .openContainer            = _openContainer,
    .openContainerWithKey     = _openContainerWithKey,
    .closeContainer           = _closeContainer,
    .append                   = _append,
    .appendKeyValue           = _appendKeyValue,
};

/*-----------------------------------------------------------*/

static void _translateErrorCode( CborError cborError,
                                 AwsIotSerializerError_t * pSerializerError )
{
    /* This function translate cbor error to serializer error.
     * It doesn't make sense that both of them are of error (greater than 0).
     */
    AwsIotSerializer_Assert( cborError == 0 || *pSerializerError == 0 );

    /* Only translate if there is no error on serizlier currently. */
    if( *pSerializerError == AWS_IOT_SERIALIZER_SUCCESS )
    {
        switch( cborError )
        {
            case CborNoError:
                *pSerializerError = AWS_IOT_SERIALIZER_SUCCESS;
                break;

            case CborErrorOutOfMemory:
                *pSerializerError = AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL;
                break;

            default:
                *pSerializerError = AWS_IOT_SERIALIZER_INTERNAL_FAILURE;
        }
    }
}

/*-----------------------------------------------------------*/

static size_t _getEncodedSize( AwsIotSerializerEncoderObject_t * pEncoderObject,
                               uint8_t * pDataBuffer )
{
    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    return cbor_encoder_get_buffer_size( pCborEncoder, pDataBuffer );
}

/*-----------------------------------------------------------*/

static size_t _getExtraBufferSizeNeeded( AwsIotSerializerEncoderObject_t * pEncoderObject )
{
    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    return cbor_encoder_get_extra_bytes_needed( pCborEncoder );
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _init( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                      uint8_t * pDataBuffer,
                                      size_t maxSize )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;

    /* Unused flags for tinycbor init. */
    int unusedCborFlags = 0;

    CborEncoder * pCborEncoder = pvPortMalloc( sizeof( CborEncoder ) );

    if( pCborEncoder != NULL )
    {
        /* Store the CborEncoder pointer to handle. */
        pEncoderObject->pHandle = pCborEncoder;

        /* Always set outmost type to AWS_IOT_SERIALIZER_CONTAINER_STREAM. */
        pEncoderObject->type = AWS_IOT_SERIALIZER_CONTAINER_STREAM;

        /* Perfomr the tinycbor init. */
        cbor_encoder_init( pCborEncoder, pDataBuffer, maxSize, unusedCborFlags );
    }
    else
    {
        /* pEncoderObject is untouched. */

        returnedError = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

static void _destroy( AwsIotSerializerEncoderObject_t * pEncoderObject )
{
    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    /* Free the memorry allocated in init function. */
    vPortFree( pCborEncoder );

    /* Reset pHandle to be NULL. */
    pEncoderObject->pHandle = NULL;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _openContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                               AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                               size_t length )
{
    /* New object must be a container of map or array. */
    if( ( pNewEncoderObject->type != AWS_IOT_SERIALIZER_CONTAINER_ARRAY ) &&
        ( pNewEncoderObject->type != AWS_IOT_SERIALIZER_CONTAINER_MAP ) )
    {
        return AWS_IOT_SERIALIZER_INVALID_INPUT;
    }

    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborEncoder * pOuterEncoder = ( CborEncoder * ) pEncoderObject->pHandle;
    CborEncoder * pInnerEncoder = pvPortMalloc( sizeof( CborEncoder ) );

    if( pInnerEncoder != NULL )
    {
        /* Store the CborEncoder pointer to handle. */
        pNewEncoderObject->pHandle = pInnerEncoder;

        switch( pNewEncoderObject->type )
        {
            case AWS_IOT_SERIALIZER_CONTAINER_MAP:
                cborError = cbor_encoder_create_map( pOuterEncoder, pInnerEncoder, length );
                break;

            case AWS_IOT_SERIALIZER_CONTAINER_ARRAY:
                cborError = cbor_encoder_create_array( pOuterEncoder, pInnerEncoder, length );
                break;

            default:
                AwsIotSerializer_Assert( 0 );
        }
    }
    else
    {
        /* pEncoderObject is untouched. */
        returnedError = AWS_IOT_SERIALIZER_OUT_OF_MEMORY;
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _openContainerWithKey( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                      const char * pKey,
                                                      AwsIotSerializerEncoderObject_t * pNewEncoderObject,
                                                      size_t length )
{
    AwsIotSerializerScalarData_t keyScalarData = AwsIotSerializer_ScalarTextString( pKey );

    AwsIotSerializerError_t returnedError = _append( pEncoderObject, keyScalarData );

    /* Buffer too small is a special error case that serialization should continue. */
    if( ( returnedError == AWS_IOT_SERIALIZER_SUCCESS ) || ( returnedError == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) )
    {
        returnedError = _openContainer( pEncoderObject, pNewEncoderObject, length );
    }

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _closeContainer( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                AwsIotSerializerEncoderObject_t * pNewEncoderObject )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborEncoder * pOuterEncoder = ( CborEncoder * ) pEncoderObject->pHandle;
    CborEncoder * pInnerEncoder = ( CborEncoder * ) pNewEncoderObject->pHandle;

    cborError = cbor_encoder_close_container( pOuterEncoder, pInnerEncoder );

    /* Free inner encoder's memory regardless the result of "close container". */
    vPortFree( pInnerEncoder );

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _append( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                        AwsIotSerializerScalarData_t scalarData )
{
    AwsIotSerializerError_t returnedError = AWS_IOT_SERIALIZER_SUCCESS;
    CborError cborError = CborNoError;

    CborEncoder * pCborEncoder = ( CborEncoder * ) pEncoderObject->pHandle;

    switch( scalarData.type )
    {
        case AWS_IOT_SERIALIZER_SCALAR_SIGNED_INT:
            cborError = cbor_encode_int( pCborEncoder, scalarData.value.signedInt );
            break;

        case AWS_IOT_SERIALIZER_SCALAR_TEXT_STRING:
            cborError = cbor_encode_text_string( pCborEncoder, ( char * ) scalarData.value.pString, scalarData.value.stringLength );
            break;

        case AWS_IOT_SERIALIZER_SCALAR_BYTE_STRING:
            cborError = cbor_encode_byte_string( pCborEncoder, scalarData.value.pString, scalarData.value.stringLength );
            break;

        case AWS_IOT_SERIALIZER_SCALAR_BOOL:
            cborError = cbor_encode_boolean( pCborEncoder, scalarData.value.booleanValue );
            break;

        case AWS_IOT_SERIALIZER_SCALAR_NULL:
            cborError = cbor_encode_null( pCborEncoder );
            break;

        default:
            returnedError = AWS_IOT_SERIALIZER_UNDEFINED_TYPE;
    }

    _translateErrorCode( cborError, &returnedError );

    return returnedError;
}

/*-----------------------------------------------------------*/

static AwsIotSerializerError_t _appendKeyValue( AwsIotSerializerEncoderObject_t * pEncoderObject,
                                                const char * pKey,
                                                AwsIotSerializerScalarData_t scalarData )
{
    AwsIotSerializerScalarData_t keyScalarData = AwsIotSerializer_ScalarTextString( pKey );
    AwsIotSerializerError_t returnedError = _append( pEncoderObject, keyScalarData );

    /* Buffer too small is a special error case that serialization should continue. */
    if( ( returnedError == AWS_IOT_SERIALIZER_SUCCESS ) || ( returnedError == AWS_IOT_SERIALIZER_BUFFER_TOO_SMALL ) )
    {
        returnedError = _append( pEncoderObject, scalarData );
    }

    return returnedError;
}

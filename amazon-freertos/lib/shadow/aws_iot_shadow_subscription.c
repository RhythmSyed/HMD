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

/**
 * @file aws_iot_shadow_subscription.c
 * @brief Implements functions for interacting with the Shadow library's
 * subscription list.
 */

/* Build using a config header, if provided. */
#ifdef AWS_IOT_CONFIG_FILE
    #include AWS_IOT_CONFIG_FILE
#endif

/* Standard includes. */
#include <string.h>

/* Shadow internal include. */
#include "private/aws_iot_shadow_internal.h"

/*-----------------------------------------------------------*/

/**
 * @brief First parameter to #_shadowSubscription_match.
 */
typedef struct _thingName
{
    const char * pThingName; /**< @brief Thing Name to compare. */
    size_t thingNameLength;  /**< @brief Length of `pThingName`. */
} _thingName_t;

/*-----------------------------------------------------------*/

/**
 * @brief Match two #_shadowSubscription_t by Thing Name.
 *
 * @param[in] pSubscriptionLink Pointer to the link member of a #_shadowSubscription_t
 * containing the Thing Name to check.
 * @param[in] pMatch Pointer to a #_thingName_t.
 *
 * @return `true` if the Thing Names match; `false` otherwise.
 */
static bool _shadowSubscription_match( const IotLink_t * pSubscriptionLink,
                                       void * pMatch );

/**
 * @brief Modify Shadow subscriptions, either by unsubscribing or subscribing.
 *
 * @param[in] mqttConnection The MQTT connection to use.
 * @param[in] pTopicFilter The topic filter to modify.
 * @param[in] topicFilterLength The length of `pTopicFilter`.
 * @param[in] callback The callback function to execute for an incoming message.
 * @param[in] mqttOperation Either @ref mqtt_function_timedsubscribe or @ref
 * mqtt_function_timedunsubscribe.
 *
 * @return #AWS_IOT_SHADOW_STATUS_PENDING on success; otherwise
 * #AWS_IOT_SHADOW_NO_MEMORY or #AWS_IOT_SHADOW_MQTT_ERROR.
 */
static AwsIotShadowError_t _modifyOperationSubscriptions( AwsIotMqttConnection_t mqttConnection,
                                                          const char * const pTopicFilter,
                                                          uint16_t topicFilterLength,
                                                          _mqttCallbackFunction_t callback,
                                                          _mqttOperationFunction_t mqttOperation );

/*-----------------------------------------------------------*/

/**
 * @brief List of active Shadow subscriptions objects.
 */
IotListDouble_t _AwsIotShadowSubscriptions = { 0 };

/**
 * @brief Protects #_AwsIotShadowSubscriptions from concurrent access.
 */
AwsIotMutex_t _AwsIotShadowSubscriptionsMutex;

/*-----------------------------------------------------------*/

static bool _shadowSubscription_match( const IotLink_t * pSubscriptionLink,
                                       void * pMatch )
{
    bool match = false;
    const _shadowSubscription_t * const pSubscription = IotLink_Container( _shadowSubscription_t,
                                                                           pSubscriptionLink,
                                                                           link );
    const _thingName_t * const pThingName = ( _thingName_t * ) pMatch;

    if( pThingName->thingNameLength == pSubscription->thingNameLength )
    {
        /* Check for matching Thing Names. */
        match = ( strncmp( pThingName->pThingName,
                           pSubscription->pThingName,
                           pThingName->thingNameLength ) == 0 );
    }

    return match;
}

/*-----------------------------------------------------------*/

static AwsIotShadowError_t _modifyOperationSubscriptions( AwsIotMqttConnection_t mqttConnection,
                                                          const char * const pTopicFilter,
                                                          uint16_t topicFilterLength,
                                                          _mqttCallbackFunction_t callback,
                                                          _mqttOperationFunction_t mqttOperation )
{
    AwsIotMqttError_t mqttStatus = AWS_IOT_MQTT_STATUS_PENDING;
    AwsIotMqttSubscription_t subscription = AWS_IOT_MQTT_SUBSCRIPTION_INITIALIZER;

    /* The MQTT operation function pointer must be either Subscribe or Unsubscribe. */
    AwsIotShadow_Assert( ( mqttOperation == AwsIotMqtt_TimedSubscribe ) ||
                         ( mqttOperation == AwsIotMqtt_TimedUnsubscribe ) );

    /* Per the AWS IoT documentation, Shadow topic subscriptions are QoS 1. */
    subscription.QoS = 1;

    AwsIotLogDebug( "%s Shadow subscription for %.*s",
                    mqttOperation == AwsIotMqtt_TimedSubscribe ? "Adding" : "Removing",
                    topicFilterLength,
                    pTopicFilter );

    /* Set the members of the subscription parameter. */
    subscription.pTopicFilter = pTopicFilter;
    subscription.topicFilterLength = topicFilterLength;
    subscription.callback.param1 = NULL;
    subscription.callback.function = callback;

    /* Call the MQTT operation function. */
    mqttStatus = mqttOperation( mqttConnection,
                                &subscription,
                                1,
                                0,
                                _AwsIotShadowMqttTimeoutMs );

    /* Check the result of the MQTT operation. */
    if( mqttStatus != AWS_IOT_MQTT_SUCCESS )
    {
        AwsIotLogError( "Failed to %s %.*s, error %s.",
                        mqttOperation == AwsIotMqtt_TimedSubscribe ? "subscribe to" : "unsubscribe from",
                        topicFilterLength,
                        pTopicFilter,
                        AwsIotMqtt_strerror( mqttStatus ) );

        /* Convert the MQTT "NO MEMORY" error to a Shadow "NO MEMORY" error. */
        if( mqttStatus == AWS_IOT_MQTT_NO_MEMORY )
        {
            return AWS_IOT_SHADOW_NO_MEMORY;
        }

        return AWS_IOT_SHADOW_MQTT_ERROR;
    }
    else
    {
        AwsIotLogDebug( "Successfully %s %.*s",
                        mqttOperation == AwsIotMqtt_TimedSubscribe ? "subscribed to" : "unsubscribed from",
                        topicFilterLength,
                        pTopicFilter );
    }

    return AWS_IOT_SHADOW_STATUS_PENDING;
}

/*-----------------------------------------------------------*/

_shadowSubscription_t * AwsIotShadowInternal_FindSubscription( const char * const pThingName,
                                                               size_t thingNameLength )
{
    _shadowSubscription_t * pSubscription = NULL;
    _thingName_t thingName =
    {
        .pThingName      = pThingName,
        .thingNameLength = thingNameLength
    };

    /* Search the list for an existing subscription for Thing Name. */
    pSubscription = IotLink_Container( _shadowSubscription_t,
                                       IotListDouble_FindFirstMatch( &( _AwsIotShadowSubscriptions ),
                                                                     NULL,
                                                                     _shadowSubscription_match,
                                                                     &thingName ),
                                       link );

    /* Check if a subscription was found. */
    if( pSubscription == NULL )
    {
        /* No subscription found. Allocate a new subscription. */
        pSubscription = AwsIotShadow_MallocSubscription( sizeof( _shadowSubscription_t ) + thingNameLength );

        if( pSubscription != NULL )
        {
            /* Clear the new subscription. */
            ( void ) memset( pSubscription, 0x00, sizeof( _shadowSubscription_t ) + thingNameLength );

            /* Set the Thing Name length and copy the Thing Name into the new subscription. */
            pSubscription->thingNameLength = thingNameLength;
            ( void ) strncpy( pSubscription->pThingName, pThingName, thingNameLength );

            /* Add the new subscription to the subscription list. */
            IotListDouble_InsertHead( &( _AwsIotShadowSubscriptions ),
                                      &( pSubscription->link ) );

            AwsIotLogDebug( "Created new Shadow subscriptions object for %.*s.",
                            thingNameLength,
                            pThingName );
        }
        else
        {
            AwsIotLogError( "Failed to allocate memory for %.*s Shadow subscriptions.",
                            thingNameLength,
                            pThingName );
        }
    }
    else
    {
        AwsIotLogDebug( "Found existing Shadow subscriptions object for %.*s.",
                        thingNameLength,
                        pThingName );
    }

    return pSubscription;
}

/*-----------------------------------------------------------*/

void AwsIotShadowInternal_RemoveSubscription( _shadowSubscription_t * const pSubscription,
                                              _shadowSubscription_t ** const pRemovedSubscription )
{
    int i = 0;

    AwsIotLogDebug( "Checking if subscription object for %.*s can be removed.",
                    pSubscription->thingNameLength,
                    pSubscription->pThingName );

    /* If any Shadow operation's subscription reference count is not 0, then the
     * subscription cannot be removed. */
    for( i = 0; i < _SHADOW_OPERATION_COUNT; i++ )
    {
        if( pSubscription->references[ i ] > 0 )
        {
            AwsIotLogDebug( "Reference count %d for %.*s subscription object. "
                            "Subscription cannot be removed yet.",
                            pSubscription->references[ i ],
                            pSubscription->thingNameLength,
                            pSubscription->pThingName );

            return;
        }
        else if( pSubscription->references[ i ] == _PERSISTENT_SUBSCRIPTION )
        {
            AwsIotLogDebug( "Subscription object for %.*s has persistent subscriptions. "
                            "Subscription will not be removed.",
                            pSubscription->thingNameLength,
                            pSubscription->pThingName );

            return;
        }
    }

    /* If any Shadow callbacks are active, then the subscription cannot be removed. */
    for( i = 0; i < _SHADOW_CALLBACK_COUNT; i++ )
    {
        if( pSubscription->callbacks[ i ].function != NULL )
        {
            AwsIotLogDebug( "Found active Shadow %s callback for %.*s subscription object. "
                            "Subscription cannot be removed yet.",
                            _pAwsIotShadowCallbackNames[ i ],
                            pSubscription->thingNameLength,
                            pSubscription->pThingName );

            return;
        }
    }

    /* No Shadow operation subscription references or active Shadow callbacks.
     * Remove the subscription object. */
    IotListDouble_Remove( &( pSubscription->link ) );

    AwsIotLogDebug( "Removed subscription object for %.*s.",
                    pSubscription->thingNameLength,
                    pSubscription->pThingName );

    /* If the caller requested the removed subscription, set the output parameter.
     * Otherwise, free the memory used by the subscription. */
    if( pRemovedSubscription != NULL )
    {
        *pRemovedSubscription = pSubscription;
    }
    else
    {
        AwsIotShadowInternal_DestroySubscription( pSubscription );
    }
}

/*-----------------------------------------------------------*/

void AwsIotShadowInternal_DestroySubscription( void * pData )
{
    _shadowSubscription_t * pSubscription = ( _shadowSubscription_t * ) pData;

    /* Free the topic buffer. It should not be NULL. */
    AwsIotShadow_Assert( pSubscription->pTopicBuffer != NULL );
    AwsIotShadow_FreeString( pSubscription->pTopicBuffer );

    /* Free memory used by subscription. */
    AwsIotShadow_FreeSubscription( pSubscription );
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t AwsIotShadowInternal_IncrementReferences( _shadowOperation_t * const pOperation,
                                                              char * const pTopicBuffer,
                                                              uint16_t operationTopicLength,
                                                              _mqttCallbackFunction_t callback )
{
    uint16_t topicFilterLength = 0;
    AwsIotShadowError_t status = AWS_IOT_SHADOW_STATUS_PENDING;
    const _shadowOperationType_t type = pOperation->type;
    _shadowSubscription_t * const pSubscription = pOperation->pSubscription;

    /* Do nothing if this operation has persistent subscriptions. */
    if( pSubscription->references[ type ] == _PERSISTENT_SUBSCRIPTION )
    {
        AwsIotLogDebug( "Shadow %s for %.*s has persistent subscriptions. Reference "
                        "count will not be incremented.",
                        _pAwsIotShadowOperationNames[ type ],
                        pSubscription->thingNameLength,
                        pSubscription->pThingName );

        return AWS_IOT_SHADOW_STATUS_PENDING;
    }

    /* When persistent subscriptions are not present, the reference count must
     * not be negative. */
    AwsIotShadow_Assert( pSubscription->references[ type ] >= 0 );

    /* Check if there are any existing references for this operation. */
    if( pSubscription->references[ type ] == 0 )
    {
        /* Place the topic "accepted" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         _SHADOW_ACCEPTED_SUFFIX,
                         _SHADOW_ACCEPTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + _SHADOW_ACCEPTED_SUFFIX_LENGTH );

        /* There should not be an active subscription for the accepted topic. */
        AwsIotShadow_Assert( AwsIotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                      pTopicBuffer,
                                                      topicFilterLength,
                                                      NULL ) == false );

        /* Add a subscription to the Shadow "accepted" topic. */
        status = _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                callback,
                                                AwsIotMqtt_TimedSubscribe );

        if( status != AWS_IOT_SHADOW_STATUS_PENDING )
        {
            return status;
        }

        /* Place the topic "rejected" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         _SHADOW_REJECTED_SUFFIX,
                         _SHADOW_REJECTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + _SHADOW_REJECTED_SUFFIX_LENGTH );

        /* There should not be an active subscription for the rejected topic. */
        AwsIotShadow_Assert( AwsIotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                      pTopicBuffer,
                                                      topicFilterLength,
                                                      NULL ) == false );

        /* Add a subscription to the Shadow "rejected" topic. */
        status = _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                callback,
                                                AwsIotMqtt_TimedSubscribe );

        if( status != AWS_IOT_SHADOW_STATUS_PENDING )
        {
            /* Failed to add subscription to Shadow "rejected" topic. Remove
             * subscription for the Shadow "accepted" topic. */
            ( void ) memcpy( pTopicBuffer + operationTopicLength,
                             _SHADOW_ACCEPTED_SUFFIX,
                             _SHADOW_ACCEPTED_SUFFIX_LENGTH );
            topicFilterLength = ( uint16_t ) ( operationTopicLength + _SHADOW_ACCEPTED_SUFFIX_LENGTH );

            ( void ) _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                    pTopicBuffer,
                                                    topicFilterLength,
                                                    callback,
                                                    AwsIotMqtt_TimedUnsubscribe );

            return status;
        }
    }

    /* Increment the number of subscription references for this operation when
     * the keep subscriptions flag is not set. */
    if( ( pOperation->flags & AWS_IOT_SHADOW_FLAG_KEEP_SUBSCRIPTIONS ) == 0 )
    {
        ( pSubscription->references[ type ] )++;

        AwsIotLogDebug( "Shadow %s subscriptions for %.*s now has count %d.",
                        _pAwsIotShadowOperationNames[ type ],
                        pSubscription->thingNameLength,
                        pSubscription->pThingName,
                        pSubscription->references[ type ] );
    }
    /* Otherwise, set the persistent subscriptions flag. */
    else
    {
        pSubscription->references[ type ] = _PERSISTENT_SUBSCRIPTION;

        AwsIotLogDebug( "Set persistent subscriptions flag for Shadow %s of %.*s.",
                        _pAwsIotShadowOperationNames[ type ],
                        pSubscription->thingNameLength,
                        pSubscription->pThingName );
    }

    return status;
}

/*-----------------------------------------------------------*/

void AwsIotShadowInternal_DecrementReferences( _shadowOperation_t * const pOperation,
                                               char * const pTopicBuffer,
                                               _shadowSubscription_t ** const pRemovedSubscription )
{
    uint16_t topicFilterLength = 0;
    const _shadowOperationType_t type = pOperation->type;
    _shadowSubscription_t * const pSubscription = pOperation->pSubscription;
    uint16_t operationTopicLength = 0;

    /* Do nothing if this Shadow operation has persistent subscriptions. */
    if( pSubscription->references[ type ] == _PERSISTENT_SUBSCRIPTION )
    {
        AwsIotLogDebug( "Shadow %s for %.*s has persistent subscriptions. Reference "
                        "count will not be decremented.",
                        _pAwsIotShadowOperationNames[ type ],
                        pSubscription->thingNameLength,
                        pSubscription->pThingName );

        return;
    }

    /* Decrement the number of subscription references for this operation.
     * Ensure that it's positive. */
    ( pSubscription->references[ type ] )--;
    AwsIotShadow_Assert( pSubscription->references[ type ] >= 0 );

    /* Check if the number of references has reached 0. */
    if( pSubscription->references[ type ] == 0 )
    {
        AwsIotLogDebug( "Reference count for %.*s %s is 0. Unsubscribing.",
                        pSubscription->thingNameLength,
                        pSubscription->pThingName,
                        _pAwsIotShadowOperationNames[ type ] );

        /* Subscription must have a topic buffer. */
        AwsIotShadow_Assert( pSubscription->pTopicBuffer != NULL );

        /* Generate the prefix of the Shadow topic. This function will not
         * fail when given a buffer. */
        ( void ) AwsIotShadowInternal_GenerateShadowTopic( ( _shadowOperationType_t ) type,
                                                           pSubscription->pThingName,
                                                           pSubscription->thingNameLength,
                                                           &( pSubscription->pTopicBuffer ),
                                                           &operationTopicLength );

        /* Place the topic "accepted" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         _SHADOW_ACCEPTED_SUFFIX,
                         _SHADOW_ACCEPTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + _SHADOW_ACCEPTED_SUFFIX_LENGTH );

        /* There should be an active subscription for the accepted topic. */
        AwsIotShadow_Assert( AwsIotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                      pTopicBuffer,
                                                      topicFilterLength,
                                                      NULL ) == true );

        /* Remove the subscription from the Shadow "accepted" topic. */
        ( void ) _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                NULL,
                                                AwsIotMqtt_TimedUnsubscribe );

        /* Place the topic "rejected" suffix at the end of the Shadow topic buffer. */
        ( void ) memcpy( pTopicBuffer + operationTopicLength,
                         _SHADOW_REJECTED_SUFFIX,
                         _SHADOW_REJECTED_SUFFIX_LENGTH );
        topicFilterLength = ( uint16_t ) ( operationTopicLength + _SHADOW_ACCEPTED_SUFFIX_LENGTH );

        /* There should be an active subscription for the accepted topic. */
        AwsIotShadow_Assert( AwsIotMqtt_IsSubscribed( pOperation->mqttConnection,
                                                      pTopicBuffer,
                                                      topicFilterLength,
                                                      NULL ) == true );

        /* Remove the subscription from the Shadow "rejected" topic. */
        ( void ) _modifyOperationSubscriptions( pOperation->mqttConnection,
                                                pTopicBuffer,
                                                topicFilterLength,
                                                NULL,
                                                AwsIotMqtt_TimedUnsubscribe );
    }

    /* Check if this subscription should be deleted. */
    AwsIotShadowInternal_RemoveSubscription( pSubscription,
                                             pRemovedSubscription );
}

/*-----------------------------------------------------------*/

AwsIotShadowError_t AwsIotShadow_RemovePersistentSubscriptions( AwsIotMqttConnection_t mqttConnection,
                                                                const char * const pThingName,
                                                                size_t thingNameLength,
                                                                uint32_t flags )
{
    int i = 0;
    uint16_t operationTopicLength = 0, topicFilterLength = 0;
    AwsIotShadowError_t removeAcceptedStatus = AWS_IOT_SHADOW_STATUS_PENDING,
                        removeRejectedStatus = AWS_IOT_SHADOW_STATUS_PENDING;
    _shadowSubscription_t * pSubscription = NULL;
    _thingName_t thingName =
    {
        .pThingName      = pThingName,
        .thingNameLength = thingNameLength
    };

    AwsIotLogInfo( "Removing persistent subscriptions for %.*s.",
                   thingNameLength,
                   pThingName );

    AwsIotMutex_Lock( &( _AwsIotShadowSubscriptionsMutex ) );

    /* Search the list for an existing subscription for Thing Name. */
    pSubscription = IotLink_Container( _shadowSubscription_t,
                                       IotListDouble_FindFirstMatch( &( _AwsIotShadowSubscriptions ),
                                                                     NULL,
                                                                     _shadowSubscription_match,
                                                                     &thingName ),
                                       link );

    /* Unsubscribe from operation subscriptions if found. */
    if( pSubscription != NULL )
    {
        AwsIotLogDebug( "Found subscription object for %.*s. Checking for persistent "
                        "subscriptions to remove.",
                        thingNameLength,
                        pThingName );

        for( i = 0; i < _SHADOW_OPERATION_COUNT; i++ )
        {
            if( ( flags & ( 0x1UL << i ) ) != 0 )
            {
                AwsIotLogDebug( "Removing %.*s %s persistent subscriptions.",
                                thingNameLength,
                                pThingName,
                                _pAwsIotShadowOperationNames[ i ] );

                /* Subscription must have a topic buffer. */
                AwsIotShadow_Assert( pSubscription->pTopicBuffer != NULL );

                if( pSubscription->references[ i ] == _PERSISTENT_SUBSCRIPTION )
                {
                    /* Generate the prefix of the Shadow topic. This function will not
                     * fail when given a buffer. */
                    ( void ) AwsIotShadowInternal_GenerateShadowTopic( ( _shadowOperationType_t ) i,
                                                                       pThingName,
                                                                       thingNameLength,
                                                                       &( pSubscription->pTopicBuffer ),
                                                                       &operationTopicLength );

                    /* Remove the "accepted" topic. */
                    ( void ) memcpy( pSubscription->pTopicBuffer + operationTopicLength,
                                     _SHADOW_ACCEPTED_SUFFIX,
                                     _SHADOW_ACCEPTED_SUFFIX_LENGTH );
                    topicFilterLength = ( uint16_t ) ( operationTopicLength + _SHADOW_ACCEPTED_SUFFIX_LENGTH );

                    removeAcceptedStatus = _modifyOperationSubscriptions( mqttConnection,
                                                                          pSubscription->pTopicBuffer,
                                                                          topicFilterLength,
                                                                          NULL,
                                                                          AwsIotMqtt_TimedUnsubscribe );

                    if( removeAcceptedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
                    {
                        break;
                    }

                    /* Remove the "rejected" topic. */
                    ( void ) memcpy( pSubscription->pTopicBuffer + operationTopicLength,
                                     _SHADOW_REJECTED_SUFFIX,
                                     _SHADOW_ACCEPTED_SUFFIX_LENGTH );
                    topicFilterLength = ( uint16_t ) ( operationTopicLength +
                                                       _SHADOW_REJECTED_SUFFIX_LENGTH );

                    removeRejectedStatus = _modifyOperationSubscriptions( mqttConnection,
                                                                          pSubscription->pTopicBuffer,
                                                                          topicFilterLength,
                                                                          NULL,
                                                                          AwsIotMqtt_TimedUnsubscribe );

                    if( removeRejectedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
                    {
                        break;
                    }

                    /* Clear the persistent subscriptions flag. */
                    pSubscription->references[ i ] = 0;
                }
                else
                {
                    AwsIotLogDebug( "%.*s %s does not have persistent subscriptions.",
                                    thingNameLength,
                                    pThingName,
                                    _pAwsIotShadowOperationNames[ i ] );
                }
            }
        }
    }
    else
    {
        AwsIotLogWarn( "No subscription object found for %.*s",
                       thingNameLength,
                       pThingName );
    }

    AwsIotMutex_Unlock( &( _AwsIotShadowSubscriptionsMutex ) );

    /* Check the results of the MQTT unsubscribes. */
    if( removeAcceptedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
    {
        return removeAcceptedStatus;
    }

    if( removeRejectedStatus != AWS_IOT_SHADOW_STATUS_PENDING )
    {
        return removeRejectedStatus;
    }

    return AWS_IOT_SHADOW_SUCCESS;
}

/*-----------------------------------------------------------*/

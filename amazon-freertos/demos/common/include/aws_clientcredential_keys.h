/*
 * Amazon FreeRTOS V1.4.1
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

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

#include <stdint.h>

/*
 * PEM-encoded client certificate
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDWTCCAkGgAwIBAgIULdb2ckcKFZ8YQrGxwJZHG93tq8wwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDEyNzIyMzA0\n"\
"MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMTjAQUjW451pZ8vKizt\n"\
"OqWLkxd4jo55XDPhhu7eflN4U/mkTvmlrEHAsPyQDwomvEmSfj67S1HAfRzbKzYc\n"\
"l2xY+mkqb6ZL92KyG6kKAQZ/EATytqZob1gW0CThAAhhHmKr+8Vh0OCVVqc/sA/s\n"\
"uf52qEZW/ZFcjqXXCHQN3jXM83cxnE9uWFJB4Ys/z4BCXVqHrWcPfpNu36xyDhwb\n"\
"Ke9QKYsk7mr7XjTNgtHon2GdhJle81miaj2T2jq94XxyCH3frY4s2ElBf6z1uRlP\n"\
"enNO3aaWO07RpIKvwB54tyyCzrfFWcBcne1vBWjzIBjsi/2ExGtX7oRB5Fu4WXKl\n"\
"Fg8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUVbc16SfF4HcD7gzeu52KTCPF3mcwHQYD\n"\
"VR0OBBYEFPwJx+3xryEcQ8YGt7MBIJkWLD41MAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCyTe26uqbFT7ntLTcIld8HrZYI\n"\
"7grVIOVjuwAunnqlS29Y8eycslQEi+KS5n9Iq4yoxVwdIVYGufUC424l1XbrlFI+\n"\
"qI1T32kmTvFsBOdSY0UL9VV3OpfXVmhKAvqIktX2Wx7ciEII3okEgN2GPTVhpo5p\n"\
"9g6oyb8FuTH45MgDXz96lJQsVfLtsyrJUVGhwDUBEz5/tg26u/Yp1I57a3VEXKZk\n"\
"RFUViiOHg4WH6VxktlhGlnLMrzu8mVBkpBa+XLDAZUG+WtGwgAlNU0dQGVU6sxF/\n"\
"GzaGeRZAG9mPrUGBHHw5X9/qeE0dplNw0G3qjOghUB29uyfaStX3CWLA1wBN\n"\
"-----END CERTIFICATE-----\n"

/*
 * PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 * This is required if you're using JITR, since the issuer (Certificate 
 * Authority) of the client certificate is used by the server for routing the 
 * device's initial request. (The device client certificate must always be 
 * sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html, 
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 * 
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM  NULL

/*
 * PEM-encoded client private key.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM \
"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpAIBAAKCAQEAxOMBBSNbjnWlny8qLO06pYuTF3iOjnlcM+GG7t5+U3hT+aRO\n"\
"+aWsQcCw/JAPCia8SZJ+PrtLUcB9HNsrNhyXbFj6aSpvpkv3YrIbqQoBBn8QBPK2\n"\
"pmhvWBbQJOEACGEeYqv7xWHQ4JVWpz+wD+y5/naoRlb9kVyOpdcIdA3eNczzdzGc\n"\
"T25YUkHhiz/PgEJdWoetZw9+k27frHIOHBsp71ApiyTuavteNM2C0eifYZ2EmV7z\n"\
"WaJqPZPaOr3hfHIIfd+tjizYSUF/rPW5GU96c07dppY7TtGkgq/AHni3LILOt8VZ\n"\
"wFyd7W8FaPMgGOyL/YTEa1fuhEHkW7hZcqUWDwIDAQABAoIBAH2yL0SIfjtFAKfQ\n"\
"BK0KBxcJNK8XWGu9LxAwO2TkxuGjQHlwXLsk+gRAcL3NesyQTkTqMY77I+rdaQKz\n"\
"KbMyLboOVFAvt3gDp7xq2R3iNSVgmWmPSuySYANtDRV+T/cJicyU3/fzqta2gMvl\n"\
"7RE9oMDEIEpH3zqeTLOzxMHS2LqfLkCeC8DKkjB7DKcY0zUJWixiwr2H7DA/yko8\n"\
"yqBr4gdNrTricy3lboQySGNxXZ0/6BykF+KKPsjPnTkLYxXNOwuEaqKAQm3edvBz\n"\
"pE0IFc7ArqbedGXhZEU85ATBpLSSNzi6CwBC66oEBOqmcLYhyafIe1We9C0m5XMA\n"\
"4s/kLwECgYEA9YcwVfBay9xuAu3/IfJ0NWR1eNt/HBlBvMaJZU4murik0iw3lTIG\n"\
"CcMUMenfrcWaw0z7+TwZk4m/cjIQxlizcse2S9uq0DKrp+vq9EWHvyCfa3gQkwMH\n"\
"3cGt70qfifc1i9JsfxIUmj+HW1BbuFei8T6pG+7FYYnxPXpzhmNPOucCgYEAzUi4\n"\
"2n8MxosWjgg4ZD4KXhAhYnT7hauKdyA2QwJKLU8+Qa7pJpuV5wjfIAVYIsMWNCcD\n"\
"Zu+boHLvGGQPH9TWd2hvVxUF91v9GC1ASg+E8uLyCB0EK0FgLnvih7BG9XeJGhqY\n"\
"XcykWx6vFi6lh+LZzsSe8W5pkeV+QX8KJhJTzpkCgYEA2iVG+NQYl9A9njS+3faA\n"\
"8X9UMBE1Gk4Gbicno+6P0o6DQokuyUwsuTeuFbrplYdgPe2EYqjXQJK+mkOJS/E7\n"\
"Xtu6SCPf3CNpCEU/99BfD514hw+iCjxDHPAcDeOLZBVUaeqHo075JyMSrDr1gw+6\n"\
"IgW+UqcOj97zi9/kDJ7nojkCgYEAowD2v5CYzWKOE1PVq0tUIiK+z5ZCTn7K4RKz\n"\
"43uC+P1uWuuPeLecTSrPOXbk6B1umRTNb8QdGEeLMRvqooT3kzNbDZPE/AiQLM1p\n"\
"uiUelVq9gtG/iLVNsTtvwfyoK6uyMwchK4Iek7gGoR+DEyZ/et7Hsr/QlifbnBhL\n"\
"L7Xn1CkCgYBXbmaoGvsdZAHhrJXqdU5d/l4sBaErrXS+8dcDuTJua7XmBLJfKJ+w\n"\
"rXxnKyCB0ONQVwm+FCwpmgIyL0YHjIQzrw96nV3ZsD8g+peZ1d9pUCS5pY3R8geK\n"\
"TodPLIyBrt1Xh+fq/tUaCrF3nBWMmZKwyFK4hvMChFt6FWn3AKyPXA==\n"\
"-----END RSA PRIVATE KEY-----\n"

/* The constants above are set to const char * pointers defined in aws_demo_runner.c,
 * and externed here for use in C files.  NOTE!  THIS IS DONE FOR CONVENIENCE
 * DURING AN EVALUATION PHASE AND IS NOT GOOD PRACTICE FOR PRODUCTION SYSTEMS 
 * WHICH MUST STORE KEYS SECURELY. */
extern const char clientcredentialCLIENT_CERTIFICATE_PEM[];
extern const char* clientcredentialJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM;
extern const char clientcredentialCLIENT_PRIVATE_KEY_PEM[];
extern const uint32_t clientcredentialCLIENT_CERTIFICATE_LENGTH;
extern const uint32_t clientcredentialCLIENT_PRIVATE_KEY_LENGTH;

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */

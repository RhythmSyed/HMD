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
"MIIDWTCCAkGgAwIBAgIUEbDlm77whC2L1Vvsdag/64gJtUQwDQYJKoZIhvcNAQEL\n"\
"BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"\
"SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTE5MDEyNTIyMTgy\n"\
"NVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"\
"ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMLn0xGitpnQ/dJAgb1y\n"\
"q2PnzBttYOdGSqvjM7g+J3D/lKWOyZfN7ZKOpAfK8HLgeR3h5DPSX1UFXNFXEZoJ\n"\
"76u3HygzbLRzvV+8tJyD0E4SUu7AgRfSLKyPFM5+0t9SrNFT1tUwmnqHvbXgtU2d\n"\
"/SzHV/fCpmvxzngEocr3tDMWD58Z88nVfJD8dtuakMyy+rVXz9c+RDHFnXdwUNv5\n"\
"SRzuhJ3pSf70I1u14xZs6tgiH9gspshPkGN1hTNxGDlgJk7ORwCDCLpCrJWHxtla\n"\
"k0bU8A6ZRxhChXF+AGQYD3O2lwNXYluCXO3sU2IYFevWqmOyPtYg3CbiWMFXsW/F\n"\
"rY8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUQVZNyvuX1CZgRVEWcL618ikLfVgwHQYD\n"\
"VR0OBBYEFB/zpmCWoQ7ADL+RlIagEouS0dMUMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"\
"AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQBlUru9h6IEa0mC5UEFtA0T8gpn\n"\
"g5MvfO1qczgfdkWki8+ajJS/49uhaxhWS2r4huWKvwv11XYzfAzVWo/AqjmHI4rj\n"\
"z9k/Wodfn5ljJ3+9W4s/5FfoTNXHUp+yOyTNov1tsSdhCNLxu7rD01ShQu2bckvp\n"\
"XWha4asid50A2yUs3+CzVvhWRQhnM4UdJEQFA9Vf+v7yldEfSLaeYHihvriw7hPh\n"\
"nzDRND/EExkYvvt5+ueM5vqrtMB4keV05GHuzS8FRG64TkU3rvFG/R6Q59cdJ5oO\n"\
"ZzysqC4h/TWvULso6JimAmBWYQhOFWLN1IXMzpmxCM1bnBA+sZVX+lWVa2Lp\n"\
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
"MIIEowIBAAKCAQEAwufTEaK2mdD90kCBvXKrY+fMG21g50ZKq+MzuD4ncP+UpY7J\n"\
"l83tko6kB8rwcuB5HeHkM9JfVQVc0VcRmgnvq7cfKDNstHO9X7y0nIPQThJS7sCB\n"\
"F9IsrI8Uzn7S31Ks0VPW1TCaeoe9teC1TZ39LMdX98Kma/HOeAShyve0MxYPnxnz\n"\
"ydV8kPx225qQzLL6tVfP1z5EMcWdd3BQ2/lJHO6EnelJ/vQjW7XjFmzq2CIf2Cym\n"\
"yE+QY3WFM3EYOWAmTs5HAIMIukKslYfG2VqTRtTwDplHGEKFcX4AZBgPc7aXA1di\n"\
"W4Jc7exTYhgV69aqY7I+1iDcJuJYwVexb8WtjwIDAQABAoIBACyRajhcJrkhiwjq\n"\
"6z/w2rQrqwaSilgYQLbxa0A0Jjy2lL0O25Y05qBesa9+fmEmZ0un/dDDyDzezwyn\n"\
"hkTdBMsVQrCkobTZynH76qjnk1hm5EPoDRzvWHvU9AMMCRg8BXyQWU3CoXBnXcAb\n"\
"NBodiGkW5EypW/jPAFF9banF3pTXjlR+bT6G8BdHax9ivk+OzrVgIrit/5PLIirB\n"\
"z38d6HR2ottHxvJ0wzo7V5HAmxJtDZNA9Ys4HnTIGeVwhfKw/9xG+hYXEX1xDjU2\n"\
"8lgXistQSAOIBhOT1pbsubiLcPxEQVBJytSn3t5KAVkz5iXs+4nWFUEJOKDua2Ij\n"\
"cBWI/jECgYEA6ZtNSXmhpuve/yXYBbQ508Js66zQ6eFFXZpq1wJK1NIQjQLZ7ANZ\n"\
"W5JgSJ+jF2OWIHH3RQ4FLNGvPY4cCOeBhcqNh0yoYa1b6n3g9xLZWFwrEZvac5bd\n"\
"qXFl4XBDYU8H9sRqD5KimGQ9S6DdFAlGEPvFcUULdGvvFaD6C71U1a0CgYEA1ZbM\n"\
"q3VnwVAe085COVVEq3kAaeic1oryDfMpLJ7+67GMkCejmyszpBnaIjfI47NFpsT5\n"\
"RYER0R81gp9y3MTJBnGeWvcbD6fU09HMVSOBJYx8X1KYZaMMVZ2SzBSBza7KelAp\n"\
"XeF2CNTVD/fOIkbSCOhBjFxBjWlywu3SSJSHH6sCgYEAvU6aKiHEKxXYB2Iulii/\n"\
"9+gGBlJslit03+7EB+sWozJ8MRmPaeAo6LlNB20+aDTOswpH/JDpBrIB1ds0ws6A\n"\
"uuAzIuKvTUCETRmnVfHTafR+/m0TcTpf9PEtQ6yLHe9Cn+n5nRcSZnZM6d25aoQ9\n"\
"YIP3M9xk9PqGrDtghmTuu50CgYB4RbpqsmoCiUNSscrIziMlC7YX7Uyj4wn6oPqG\n"\
"jeGuhwRICIowTlt+BLC4pAETd1e5WSGk6dFGmPRg4bikaItqfYBWaN9sKW3G+VS+\n"\
"fIe0xS8j3jbCxNWlQF3xXXraf5q01X71xofY4UA5Baml04tiSKbgivK+1qow6gqH\n"\
"pFuSkQKBgDCimMg2jBnSyzZbqCzkPiaryv7gO1diCdeeiz2dHsLZu8evdNj8Smx9\n"\
"fM6rWoyFE9jSX55u1KKSjaBfhoo+X5NnRSjEYBSwF1v9xJLC+qlOIsc63PvykqUG\n"\
"WUsJQ5rBWnPLDZJF1ny3eU25jO0ZJkIkCWKNG+AzsU7UW3wbzpkg\n"\
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

/*
 * Copyright 2013-2016 Guardtime, Inc.
 *
 * This file is part of the Guardtime client SDK.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES, CONDITIONS, OR OTHER LICENSES OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 * "Guardtime" and "KSI" are trademarks or registered trademarks of
 * Guardtime, Inc., and no license to trademarks is granted; Guardtime
 * reserves and retains all trademark rights.
 */

#include <ksi/pkitruststore.h>
#include <ksi/policy.h>
#include <string.h>
#include <stdlib.h>
#include "obj_printer.h"
#include "api_wrapper.h"

void OBJPRINT_publicationsFileReferences(const KSI_PublicationsFile *pubFile, int (*print)(const char *format, ... )){
	int res = KSI_UNKNOWN_ERROR;
	KSI_LIST(KSI_PublicationRecord)* list_publicationRecord = NULL;
	KSI_PublicationRecord *publicationRecord = NULL;
	char buf[1024];
	unsigned int i, j;
	char *pLineBreak = NULL;
	char *pStart = NULL;

	if (pubFile == NULL) return;

	print("Publication Records:\n");

	res = KSI_PublicationsFile_getPublications(pubFile, &list_publicationRecord);
	if (res != KSI_OK) return;

	for (i = 0; i < KSI_PublicationRecordList_length(list_publicationRecord); i++) {
		int h=0;
		res = KSI_PublicationRecordList_elementAt(list_publicationRecord, i, &publicationRecord);
		if (res != KSI_OK) return;

		if (KSITOOL_PublicationRecord_toString(publicationRecord, buf, sizeof(buf)) == NULL) return;

		pStart = buf;
		j=1;
		h=0;
		if (i) print("\n");
		while ((pLineBreak = strchr(pStart, '\n')) != NULL){
			*pLineBreak = 0;
			if (h++ < 3)
				print("%s %s\n", "  ", pStart);
			else
				print("%s %2i) %s\n", "    ", j++, pStart);
			pStart = pLineBreak+1;
		}

		if (h < 3)
			print("%s %s\n", "  ", pStart);
		else
			print("%s %2i) %s\n", "    ", j++, pStart);
	}
	print("\n");
	return;
}

void OBJPRINT_signaturePublicationReference(KSI_Signature *sig, int (*print)(const char *format, ... )){
	int res = KSI_UNKNOWN_ERROR;
	KSI_PublicationRecord *publicationRecord;
	char buf[1024];
	char *pLineBreak = NULL;
	char *pStart = buf;
	int i=1;
	int h=0;
	if (sig == NULL) return;

	print("Publication Record:\n");
	res = KSI_Signature_getPublicationRecord(sig, &publicationRecord);
	if (res != KSI_OK)return ;

	if (publicationRecord == NULL) {
		print("  (No publication records available)\n\n");
		return;
	}

	if (KSITOOL_PublicationRecord_toString(publicationRecord, buf, sizeof(buf)) == NULL) return;
	pStart = buf;

	while ((pLineBreak = strchr(pStart, '\n')) != NULL){
		*pLineBreak = 0;

		if (h < 3)
			print("%s %s\n", "  ", pStart);
		else
			print("%s %2i) %s\n", "    ", i++, pStart);

		pStart = pLineBreak+1;
	}

	if (h < 3)
		print("%s %s\n", "  ", pStart);
	else
		print("%s %2i) %s\n", "    ", i++, pStart);
	print("\n");

	return;
}

void OBJPRINT_Hash(KSI_DataHash *hsh, const char *prefix, int (*print)(const char *format, ... )) {
	char buf[1024];

	if (hsh == NULL) return;

	if (KSITOOL_DataHash_toString(hsh, buf, sizeof(buf)) == NULL) return;

	print("%s%s\n",
			prefix == NULL ? "" : prefix,
			buf
			);

	return;
}
void OBJPRINT_signatureInputHash(KSI_Signature *sig, int (*print)(const char *format, ... )) {
	int res;
	KSI_DataHash *hsh = NULL;

	if (sig == NULL) return;

	res = KSI_Signature_getDocumentHash(sig, &hsh);
	if (res != KSI_OK) {
		print("Input hash: Unable to extract from the signature.\n");
		return;
	}

	OBJPRINT_Hash(hsh, "Input hash: ", print);

	return;
}

void OBJPRINT_IdentityMetadata(KSI_Signature *sig, int (*print)(const char *format, ... )){
	int res = KSI_UNKNOWN_ERROR;
	KSI_HashChainLinkIdentityList *identity = NULL;
	size_t i;
	const char *offset = "    ";
	KSI_Integer *tmpInt = NULL;

	if (sig == NULL) goto cleanup;

	print("Identity Metadata: \n");
	res = KSI_Signature_getAggregationHashChainIdentity(sig, &identity);
	if (res != KSI_OK){
		print("Unable to get signer identity.\n");
		goto cleanup;
	}

	if (KSI_HashChainLinkIdentityList_length(identity) == 0) {
		print("%s'Unknown'.\n", offset);
	} else {
		for (i = 0; i < KSI_HashChainLinkIdentityList_length(identity); i++) {
			KSI_HashChainLinkIdentity *id = NULL;
			KSI_HashChainLinkIdentityType type;

			res = KSI_HashChainLinkIdentityList_elementAt(identity, i, &id);
			if (res != KSI_OK){
				print("Unable to get identity.\n");
				goto cleanup;
			}

			res = KSI_HashChainLinkIdentity_getType(id, &type);
			if (res != KSI_OK){
				print("Unable to get identity type.\n");
				goto cleanup;
			}

			if (type == KSI_IDENTITY_TYPE_LEGACY_ID) {
				KSI_Utf8String *clientId = NULL;

				res = KSI_HashChainLinkIdentity_getClientId(id, &clientId);
				if (res != KSI_OK){
					print("Unable to get client id.\n");
					goto cleanup;
				}

				print("%s%d) '%s' (legacy)\n", offset, i + 1, KSI_Utf8String_cstr(clientId));

			} else if (type == KSI_IDENTITY_TYPE_METADATA) {
				KSI_Utf8String *clientId = NULL;
				KSI_Utf8String *macId = NULL;
				KSI_Integer *seqNr = NULL;
				KSI_Integer *reqTm = NULL;

				/* ClientId is mandatory element. */
				res = KSI_HashChainLinkIdentity_getClientId(id, &clientId);
				if (res != KSI_OK || clientId == NULL){
					print("Unable to get client id.\n");
					goto cleanup;
				}
				print("%s%d) Client ID: '%s'", offset, i + 1, KSI_Utf8String_cstr(clientId));

				/* Read optional elements. */
				res = KSI_HashChainLinkIdentity_getMachineId(id, &macId);
				if (res != KSI_OK){
					print("Unable to get machine id.\n");
					goto cleanup;
				}
				res = KSI_HashChainLinkIdentity_getSequenceNr(id, &seqNr);
				if (res != KSI_OK){
					print("Unable to get sequence number.\n");
					goto cleanup;
				}
				res = KSI_HashChainLinkIdentity_getRequestTime(id, &reqTm);
				if (res != KSI_OK){
					print("Unable to get sequence number.\n");
					goto cleanup;
				}

				/* Print values if available. */
				if (macId) print(", Machine ID: '%s'", KSI_Utf8String_cstr(macId));
				if (seqNr) print(", Sequence number: '%i'", (unsigned long)KSI_Integer_getUInt64(seqNr));
				if (reqTm) print(", Request time: '%llu'", KSI_Integer_getUInt64(reqTm));

				print("\n");
			} else {
				print("%s%d) Unknown identity type.\n", offset, i + 1);
			}
		}
	}

cleanup:
	KSI_Integer_free(tmpInt);
	KSI_HashChainLinkIdentityList_free(identity);
	return;
}

void OBJPRINT_signatureSigningTime(const KSI_Signature *sig, int (*print)(const char *format, ... )) {
	int res;
	KSI_Integer *sigTime = NULL;
	unsigned long signingTime = 0;
	char date[1024];


	if (sig == NULL) {
		return;
	}


	res = KSI_Signature_getSigningTime(sig, &sigTime);
	if (res != KSI_OK) {
		return;
	}

	if (sigTime != NULL) {
		if (KSI_Integer_toDateString(sigTime, date, sizeof(date)) != date) {
			return;
		}

		signingTime = (unsigned long)KSI_Integer_getUInt64(sigTime);

		print("Signing time: (%i) %s+00:00\n",
			  signingTime, date);
	} else {
		print("Signing time: N/A\n");
	}

	return;
}

static const char *get_signature_type_from_oid(const char *oid) {
	if (strcmp(oid, "1.2.840.113549.1.1.11") == 0) {
		return "PKI";
	} else {
		return "(unknown)";
	}
}

void OBJPRINT_signatureCertificate(const KSI_Signature *sig, int (*print)(const char *format, ... )) {
	int res;
	KSI_CalendarAuthRec *calAuthRec = NULL;
	KSI_PKISignedData *pki_data = NULL;
	KSI_OctetString *ID = NULL;
	KSI_Utf8String *sig_type = NULL;

	char str_id[1024];

	char *ret;

	if (sig == NULL) goto cleanup;

	res = KSI_Signature_getCalendarAuthRec(sig, &calAuthRec);
	if (res != KSI_OK || calAuthRec == NULL) goto cleanup;

	res = KSI_CalendarAuthRec_getSignatureData(calAuthRec, &pki_data);
	if (res != KSI_OK || pki_data == NULL) goto cleanup;

	res = KSI_PKISignedData_getCertId(pki_data, &ID);
	if (res != KSI_OK || ID == NULL) goto cleanup;

	res = KSI_PKISignedData_getSigType(pki_data, &sig_type);
	if (res != KSI_OK || sig_type == NULL) goto cleanup;

	ret = KSI_OctetString_toString(ID, ':', str_id, sizeof(str_id));
	if (ret != str_id) goto cleanup;

	print("Calendar Authentication Record %s signature:\n", get_signature_type_from_oid(KSI_Utf8String_cstr(sig_type)));
	print("  Signing certificate ID: %s\n", str_id);
	print("  Signature type: %s\n", KSI_Utf8String_cstr(sig_type));

cleanup:


	return;
}

void OBJPRINT_publicationsFileCertificates(const KSI_PublicationsFile *pubfile, int (*print)(const char *format, ... )){
	KSI_CertificateRecordList *certReclist = NULL;
	KSI_CertificateRecord *certRec = NULL;
	KSI_PKICertificate *cert = NULL;
	char buf[1024];
	unsigned int i=0;
	int res = 0;

	if (pubfile == NULL) goto cleanup;
	print("Certificates for key-based signature verification:\n");

	res = KSI_PublicationsFile_getCertificates(pubfile, &certReclist);
	if (res != KSI_OK || certReclist == NULL) goto cleanup;

	for (i = 0; i < KSI_CertificateRecordList_length(certReclist); i++){
		res = KSI_CertificateRecordList_elementAt(certReclist, i, &certRec);
		if (res != KSI_OK || certRec == NULL) goto cleanup;

		res = KSI_CertificateRecord_getCert(certRec, &cert);
		if (res != KSI_OK || cert == NULL) goto cleanup;

		if (KSI_PKICertificate_toString(cert, buf, sizeof(buf)) != NULL)
			print("%s\n", buf);
	}

cleanup:

	return;
}

void OBJPRINT_publicationsFileSigningCert(KSI_PublicationsFile *pubfile, int (*print)(const char *format, ... )) {
	int res;
	KSI_PKISignature *sig = NULL;
	KSI_PKICertificate *cert = NULL;
	char buf[2048];
	char *tmp = NULL;

	res = KSI_PublicationsFile_getSignature(pubfile, &sig);
	if (res != KSI_OK) goto cleanup;

	res = KSI_PKISignature_extractCertificate(sig, &cert);
	if (res != KSI_OK) goto cleanup;

	tmp = KSI_PKICertificate_toString(cert, buf, sizeof(buf));
	if (tmp != buf) goto cleanup;

	print("Publications file signing %s", buf);
	print("\n", buf);

cleanup:

	KSI_PKICertificate_free(cert);

	return;
}

void OBJPRINT_signatureDump(KSI_Signature *sig, int (*print)(const char *format, ... )) {

	print("KSI Signature dump:\n");

	if (sig == NULL) {
		print("(null)\n");
		return;
	}

	print("  ");
	OBJPRINT_signatureInputHash(sig, print);
	print("  ");
	OBJPRINT_signatureSigningTime(sig, print);
	print("  ");
	OBJPRINT_IdentityMetadata(sig, print);
	print("  Trust anchor: ");

	if (KSITOOL_Signature_isCalendarAuthRecPresent(sig)) {
		print("Calendar Authentication Record.\n\n");
		OBJPRINT_signatureCertificate(sig, print);
	} else if (KSITOOL_Signature_isPublicationRecordPresent(sig)) {
		print("Publication Record.\n\n");
		OBJPRINT_signaturePublicationReference(sig, print);
	} else {
		print("Calendar Blockchain.\n\n");
	}

	return;
}

void OBJPRINT_publicationsFileDump(KSI_PublicationsFile *pubfile, int (*print)(const char *format, ... )) {

	print("KSI Publications file dump:\n");

	if (pubfile == NULL) {
		print("(null)\n");
		return;
	}

	OBJPRINT_publicationsFileReferences(pubfile, print);
	OBJPRINT_publicationsFileCertificates(pubfile, print);
	OBJPRINT_publicationsFileSigningCert(pubfile, print);

	return;
}


static const char *getVerificationResultCode(KSI_VerificationResultCode code) {
	switch (code) {
		case KSI_VER_RES_OK:	return "OK";
		case KSI_VER_RES_NA:	return "NA";
		case KSI_VER_RES_FAIL:	return "FAIL";
		default:			return "UNKNOWN";
	}
}

const char *OBJPRINT_getVerificationErrorCode(KSI_VerificationErrorCode code) {
	return KSI_VerificationErrorCode_toString(code);
}

const char *OBJPRINT_getVerificationErrorDescription(KSI_VerificationErrorCode code) {
	return KSI_Policy_getErrorString(code);
}

static const char *getVerificationStepDescription(size_t step) {
	switch (step) {
		case KSI_VERIFY_DOCUMENT: return "Verifying document hash.";
		case KSI_VERIFY_AGGRCHAIN_INTERNALLY: return "Verifying aggregation hash chain internal consistency.";
		case KSI_VERIFY_AGGRCHAIN_WITH_CALENDAR_CHAIN: return "Verifying aggregation hash chain root.";
		case KSI_VERIFY_CALCHAIN_INTERNALLY: return "Verifying calendar hash chain internally.";
		case KSI_VERIFY_CALCHAIN_WITH_CALAUTHREC: return "Verifying calendar hash chain authentication record.";
		case KSI_VERIFY_CALCHAIN_WITH_PUBLICATION: return "Verifying calendar chain with publication.";
		case KSI_VERIFY_CALCHAIN_ONLINE: return "Verifying signature online.";
		case KSI_VERIFY_CALAUTHREC_WITH_SIGNATURE: return "Verifying calendar authentication record.";
		case KSI_VERIFY_PUBFILE_SIGNATURE: return "Verifying publications file.";
		case KSI_VERIFY_PUBLICATION_WITH_PUBFILE: return "Verifying publication.";
		case KSI_VERIFY_PUBLICATION_WITH_PUBSTRING: return "Verifying publication with publication string.";
		default: return "Unknown step";
	}
}

static int getVerificationStepResult(size_t step, KSI_PolicyVerificationResult *result) {
	return !!(result->finalResult.stepsPerformed & result->finalResult.stepsSuccessful & step);
}

static const char *getPrintableRuleName(const char *rule) {
	static const char *rulePrefix = "KSI_VerificationRule_";
	/* Do not print the prefix of the API rule name. Full rule name is, eg: KSI_VerificationRule_DocumentHashDoesNotExist. */
	return (rule + (strstr(rule, rulePrefix) == NULL ? 0 : strlen(rulePrefix)));
}

static void printRuleVerificationResult(int (*print)(const char *format, ... ), KSI_RuleVerificationResult *result, int printRuleWhenOk) {
	print("    %s:", getVerificationResultCode(result->resultCode));
	if (result->errorCode != KSI_VER_ERR_NONE) {
		print("\t[%s]", OBJPRINT_getVerificationErrorCode(result->errorCode));
	}
	print(" %s.", OBJPRINT_getVerificationErrorDescription(result->errorCode));
	if ((result->resultCode != KSI_VER_RES_OK) ||
			(printRuleWhenOk && result->resultCode == KSI_VER_RES_OK)) {
		print("\tIn rule:");
		print("\t%s", getPrintableRuleName(result->ruleName));
	}
	print("\n");
}

void OBJPRINT_signatureVerificationResultDump(KSI_PolicyVerificationResult *result, int (*print)(const char *format, ... )) {
	int res;
	unsigned int i = 0;
	size_t step;
	size_t stepsLeft;

	if (result == NULL){
		return;
	}

	print("KSI Verification result dump:\n");
	print("  Verification abstract:\n");
	step = 1;
	stepsLeft = result->finalResult.stepsPerformed;
	do {
		if (result->finalResult.stepsPerformed & step) {
			print("    %s.. %s", getVerificationStepDescription(step),
					(getVerificationStepResult(step, result) ? "ok" : "failed"));
			print("\n");
		}
		step <<= 1;
		stepsLeft >>= 1;
	} while (stepsLeft);

	print("  Verification details:\n");
	for (i = 0; i < KSI_RuleVerificationResultList_length(result->ruleResults); i++) {
		KSI_RuleVerificationResult *tmp = NULL;

		res = KSI_RuleVerificationResultList_elementAt(result->ruleResults, i, &tmp);
		if (res != KSI_OK || tmp == NULL) {
			return;
		}
		printRuleVerificationResult(print, tmp, 1);
	}

	print("  Final result:\n");
	/* Print also rule name in case of an error. */
	printRuleVerificationResult(print, &result->finalResult, 0);

	print("\n");
	return;
}

void OBJPRINT_aggregatorConfDump(KSI_Config *config, int (*print)(const char *format, ... )) {
	int res;
	KSI_Integer *max_level = NULL;
	KSI_Integer *aggr_algo = NULL;
	KSI_Integer *aggr_period = NULL;
	KSI_Integer *max_req = NULL;
	KSI_Utf8StringList *parent_uri = NULL;
	size_t i;

	print("Aggregator configuration:\n");

	res = KSI_Config_getAggrAlgo(config, &aggr_algo);
	if (res != KSI_OK) return;
	if (aggr_algo) print("  Hash algorithm: %s\n", KSI_getHashAlgorithmName((KSI_HashAlgorithm)KSI_Integer_getUInt64(aggr_algo)));

	res = KSI_Config_getMaxLevel(config, &max_level);
	if (res != KSI_OK) return;
	if (max_level) print("  Maximum level: %d\n", KSI_Integer_getUInt64(max_level));

	res = KSI_Config_getAggrPeriod(config, &aggr_period);
	if (res != KSI_OK) return;
	if (aggr_period) print("  Aggregation period: %d\n", KSI_Integer_getUInt64(aggr_period));

	res = KSI_Config_getMaxRequests(config, &max_req);
	if (res != KSI_OK) return;
	if (max_req) print("  Maximum requests: %d\n", KSI_Integer_getUInt64(max_req));

	res = KSI_Config_getParentUri(config, &parent_uri);
	if (res != KSI_OK) return;
	if (parent_uri) {
		print("  Parent URI:\n");
		for (i = 0; i < KSI_Utf8StringList_length(parent_uri); i++) {
			KSI_Utf8String *uri = NULL;

			res = KSI_Utf8StringList_elementAt(parent_uri, i, &uri);
			if (res != KSI_OK) return;
			print("    %s\n", KSI_Utf8String_cstr(uri));
		}
	}
	print("\n");
}

void OBJPRINT_extenderConfDump(KSI_Config *config, int (*print)(const char *format, ... )) {
	int res;
	KSI_Integer *max_req = NULL;
	KSI_Utf8StringList *parent_uri = NULL;
	KSI_Integer *cal_first = NULL;
	KSI_Integer *cal_last = NULL;
	size_t i;
	char date[1024];

	print("Extender configuration:\n");

	res = KSI_Config_getCalendarFirstTime(config, &cal_first);
	if (res != KSI_OK) return;
	if (cal_first) {
		print("  Calendar first time: (%i) %s+00:00\n",
				KSI_Integer_getUInt64(cal_first),
				KSI_Integer_toDateString(cal_first, date, sizeof(date)));
	}

	res = KSI_Config_getCalendarLastTime(config, &cal_last);
	if (res != KSI_OK) return;
	if (cal_last) {
		print("  Calendar last time:  (%i) %s+00:00\n",
				KSI_Integer_getUInt64(cal_last),
				KSI_Integer_toDateString(cal_last, date, sizeof(date)));
	}

	res = KSI_Config_getMaxRequests(config, &max_req);
	if (res != KSI_OK) return;
	if (max_req) print("  Maximum requests: %d\n", KSI_Integer_getUInt64(max_req));

	res = KSI_Config_getParentUri(config, &parent_uri);
	if (res != KSI_OK) return;
	if (parent_uri) {
		print("  Parent URI:\n");
		for (i = 0; i < KSI_Utf8StringList_length(parent_uri); i++) {
			KSI_Utf8String *uri = NULL;

			res = KSI_Utf8StringList_elementAt(parent_uri, i, &uri);
			if (res != KSI_OK) return;
			print("    %s\n", KSI_Utf8String_cstr(uri));
		}
	}
	print("\n");
}

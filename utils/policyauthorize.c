/********************************************************************************/
/*										*/
/*			    PolicyAuthorize	 				*/
/*			     Written by Ken Goldman				*/
/*		       IBM Thomas J. Watson Research Center			*/
/*	      $Id: policyauthorize.c 1294 2018-08-09 19:08:34Z kgoldman $	*/
/*										*/
/* (c) Copyright IBM Corporation 2015 - 2018.					*/
/*										*/
/* All rights reserved.								*/
/* 										*/
/* Redistribution and use in source and binary forms, with or without		*/
/* modification, are permitted provided that the following conditions are	*/
/* met:										*/
/* 										*/
/* Redistributions of source code must retain the above copyright notice,	*/
/* this list of conditions and the following disclaimer.			*/
/* 										*/
/* Redistributions in binary form must reproduce the above copyright		*/
/* notice, this list of conditions and the following disclaimer in the		*/
/* documentation and/or other materials provided with the distribution.		*/
/* 										*/
/* Neither the names of the IBM Corporation nor the names of its		*/
/* contributors may be used to endorse or promote products derived from		*/
/* this software without specific prior written permission.			*/
/* 										*/
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS		*/
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT		*/
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR	*/
/* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT		*/
/* HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,	*/
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT		*/
/* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,	*/
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY	*/
/* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT		*/
/* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE	*/
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.		*/
/********************************************************************************/

/* 

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <ibmtss/tss.h>
#include <ibmtss/tssutils.h>
#include <ibmtss/tssresponsecode.h>
#include <ibmtss/Unmarshal_fp.h>

static void printUsage(void);

int verbose = FALSE;

int main(int argc, char *argv[])
{
    TPM_RC			rc = 0;
    int				i;    /* argc iterator */
    TSS_CONTEXT			*tssContext = NULL;
    PolicyAuthorize_In 		in;
    TPMI_SH_POLICY		policySession = 0;
    const char 			*approvedPolicyFilename = NULL;
    const char			*policyRefFilename = NULL;
    const char			*signingKeyNameFilename = NULL;
    const char			*ticketFilename = NULL;
    TPMI_SH_AUTH_SESSION    	sessionHandle0 = TPM_RH_NULL;
    unsigned int		sessionAttributes0 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle1 = TPM_RH_NULL;
    unsigned int		sessionAttributes1 = 0;
    TPMI_SH_AUTH_SESSION    	sessionHandle2 = TPM_RH_NULL;
    unsigned int		sessionAttributes2 = 0;
    
    setvbuf(stdout, 0, _IONBF, 0);      /* output may be going through pipe to log file */
    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "1");

    /* command line argument defaults */
    for (i=1 ; (i<argc) && (rc == 0) ; i++) {
	if (strcmp(argv[i],"-ha") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &policySession);
	    }
	    else {
		printf("Missing parameter for -ha\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-appr") == 0) {
	    i++;
	    if (i < argc) {
		approvedPolicyFilename = argv[i];
	    }
	    else {
		printf("-appr option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-pref") == 0) {
	    i++;
	    if (i < argc) {
		policyRefFilename = argv[i];
	    }
	    else {
		printf("-pref option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-skn") == 0) {
	    i++;
	    if (i < argc) {
		signingKeyNameFilename = argv[i];
	    }
	    else {
		printf("-skn option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-tk") == 0) {
	    i++;
	    if (i < argc) {
		ticketFilename = argv[i];
	    }
	    else {
		printf("-tk option needs a value\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se0") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle0);
	    }
	    else {
		printf("Missing parameter for -se0\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes0);
		if (sessionAttributes0 > 0xff) {
		    printf("Out of range session attributes for -se0\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se0\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se1") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle1);
	    }
	    else {
		printf("Missing parameter for -se1\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes1);
		if (sessionAttributes1 > 0xff) {
		    printf("Out of range session attributes for -se1\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se1\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-se2") == 0) {
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionHandle2);
	    }
	    else {
		printf("Missing parameter for -se2\n");
		printUsage();
	    }
	    i++;
	    if (i < argc) {
		sscanf(argv[i],"%x", &sessionAttributes2);
		if (sessionAttributes2 > 0xff) {
		    printf("Out of range session attributes for -se2\n");
		    printUsage();
		}
	    }
	    else {
		printf("Missing parameter for -se2\n");
		printUsage();
	    }
	}
	else if (strcmp(argv[i],"-h") == 0) {
	    printUsage();
	}
	else if (strcmp(argv[i],"-v") == 0) {
	    verbose = TRUE;
	    TSS_SetProperty(NULL, TPM_TRACE_LEVEL, "2");
	}
	else {
	    printf("\n%s is not a valid option\n", argv[i]);
	    printUsage();
	}
    }
    /* validate command line parameters */
    if (policySession == 0) {
	printf("Missing parameter -ha\n");
	printUsage();
    }
    if (approvedPolicyFilename == NULL) {
	printf("Missing parameter -appr\n");
	printUsage();
    }
    if (policyRefFilename == NULL) {
	in.policyRef.b.size = 0;	/* default empty buffer */
    }
    if (signingKeyNameFilename == NULL) {
	printf("Missing parameter -skn\n");
	printUsage();
    }
    if (ticketFilename == NULL) {
	printf("Missing parameter -tk\n");
	printUsage();
    }
    /* set in parameters */
    if (rc == 0) {
	in.policySession = policySession;
    }
    if (rc == 0) {
	rc = TSS_File_Read2B(&in.approvedPolicy.b,
			     sizeof(in.approvedPolicy.t.buffer),
			     approvedPolicyFilename);
    }
    if ((rc == 0) && (policyRefFilename != NULL)) {
	rc = TSS_File_Read2B(&in.policyRef.b,
			     sizeof(in.policyRef.t.buffer),
			     policyRefFilename);
    }
    if (rc == 0) {
	rc = TSS_File_Read2B(&in.keySign.b,
			     sizeof(in.keySign.t.name),
			     signingKeyNameFilename);
    }
    if (rc == 0) {
	rc = TSS_File_ReadStructure(&in.checkTicket,
				    (UnmarshalFunction_t)TSS_TPMT_TK_VERIFIED_Unmarshalu,
				    ticketFilename);
    }
    /* Start a TSS context */
    if (rc == 0) {
	rc = TSS_Create(&tssContext);
    }
    /* call TSS to execute the command */
    if (rc == 0) {
	rc = TSS_Execute(tssContext,
			 NULL, 
			 (COMMAND_PARAMETERS *)&in,
			 NULL,
			 TPM_CC_PolicyAuthorize,
			 sessionHandle0, NULL, sessionAttributes0,
			 sessionHandle1, NULL, sessionAttributes1,
			 sessionHandle2, NULL, sessionAttributes2,
			 TPM_RH_NULL, NULL, 0);
    }
    {
	TPM_RC rc1 = TSS_Delete(tssContext);
	if (rc == 0) {
	    rc = rc1;
	}
    }
    if (rc == 0) {
	if (verbose) printf("policyauthorize: success\n");
    }
    else {
	const char *msg;
	const char *submsg;
	const char *num;
	printf("policyauthorize: failed, rc %08x\n", rc);
	TSS_ResponseCode_toString(&msg, &submsg, &num, rc);
	printf("%s%s%s\n", msg, submsg, num);
	rc = EXIT_FAILURE;
    }
    return rc;
}

static void printUsage(void)
{
    printf("\n");
    printf("policyauthorize\n");
    printf("\n");
    printf("Runs TPM2_PolicyAuthorize\n");
    printf("\n");
    printf("\t-ha\tpolicy session handle\n");
    printf("\t-appr\tfile name of digest of the policy being approved\n");
    printf("\t[-pref\tpolicyRef file] (default none)\n");
    printf("\t-skn\tsigning key Name file name\n");
    printf("\t-tk\tticket file name\n");
    printf("\n");
    printf("\t-se[0-2] session handle / attributes (default NULL)\n");
    printf("\t20\tcommand decrypt\n");
    printf("\t01\tcontinue\n");
    exit(1);	
}

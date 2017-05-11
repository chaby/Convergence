/*
    'msd' detects molecular signatures of phenotypic convergence / 
    'enr' computes GO terms enrichments of a list of genes / 

    Copyright (C) 2017 Gilles DIDIER

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/




#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Utils.h"
#include "Annotation.h"

#define BASIC_INC_BUFFER_G 100
#define BASIC_INC_BUFFER_L 500
#define MAX_NAME_SIZE 300
#define IS_SEP_COMMA(x) (c == ',' || issep(c))

typedef struct ANNOTATION_LIST {
	int annot, next;
} TypeAnnotationList;

TypeAnnotation *readAnnotation(FILE *f) {
    char c, tmpS[MAX_NAME_SIZE+1];
    int sizeBufferG, sizeBufferL, indexA, indexL, sizeA, sizeG, i;
	TypeAnnotation *an;
	TypeAnnotationList *list;
	int *start;
	TypeLexiTree *dictG, *dictA;
	
	dictG = newLexiTree();
	dictA = newLexiTree();
	an = (TypeAnnotation*) malloc(sizeof(TypeAnnotation));
	sizeA = 0;
	sizeG = 0;
	indexL = 0;
	sizeBufferG = BASIC_INC_BUFFER_G;
	sizeBufferL = BASIC_INC_BUFFER_L;
	start = (int*) malloc(sizeBufferG*sizeof(int));
	list = (TypeAnnotationList*) malloc(sizeBufferL*sizeof(TypeAnnotationList));
	for(i=0; i<sizeBufferG; i++)
		start[i] = -1;
	for(c=getc(f); c!=EOF && issepline(c); c = fgetc(f));
	while(c != EOF) { //read first line
        int i, indexG;
		if(c == '\'' || c == '"') {
			c = fgetc(f);
			for(i=0; i<MAX_NAME_SIZE && c != EOF && c != '\'' && c != '"'; i++) {
				tmpS[i] = c;
				c = fgetc(f);
			}
			if(c == '\'' || c == '"')
				c = fgetc(f);
			else
				exitProg(ErrorReading, "Missing closing \" or '...");			
		} else {
			for(i=0; i<MAX_NAME_SIZE && c != EOF && !issepline(c); i++) {
				tmpS[i] = c;
				c = fgetc(f);
			}
		}
		if(i == 0)
			exitProg(ErrorExec, "Empty name ...");
		if(i == MAX_NAME_SIZE)
			exitProg(ErrorExec, "Name too much long...");
		tmpS[i++] = '\0';
		if((indexG = findWordLexi(tmpS, dictG)) == END_INT) {
			indexG = sizeG;
			addWordLexi(tmpS, indexG, dictG);
			sizeG++;
		}
		if(sizeG >= sizeBufferG) {
			int i;
			sizeBufferG += BASIC_INC_BUFFER_G;
			start = (int*) realloc((void *) start, sizeBufferG*sizeof(int));
			for(i=sizeBufferG-BASIC_INC_BUFFER_G; i<sizeBufferG; i++)
				start[i] = -1;
		}
		for(; c != EOF && IS_SEP_COMMA(c); c = fgetc(f));
		while(!issepline(c)) {
			if(c == '\'' || c == '"') {
				c = fgetc(f);
				for(i=0; i<MAX_NAME_SIZE && c != EOF && c != '\'' && c != '"'; i++) {
					tmpS[i] = c;
					c = fgetc(f);
				}
				if(c == '\'' || c == '"')
					c = fgetc(f);
				else
					exitProg(ErrorReading, "Missing closing \" or '...");			
			} else {
				for(i=0; i<MAX_NAME_SIZE && c != EOF && !issepline(c); i++) {
					tmpS[i] = c;
					c = fgetc(f);
				}
			}
			if(i == 0)
				exitProg(ErrorExec, "Empty name ...");
			if(i == MAX_NAME_SIZE)
				exitProg(ErrorExec, "Name too much long...");
			tmpS[i++] = '\0';
			if(!((i != 2) && (tmpS[0] == 'N') && (tmpS[1] == 'A'))) {
				if((indexA = findWordLexi(tmpS, dictA)) == END_INT) {
					indexA = sizeA++;
					addWordLexi(tmpS, indexA, dictA);
				}
				if(indexL >= sizeBufferL) {
					sizeBufferL += BASIC_INC_BUFFER_L;
					list = (TypeAnnotationList*) realloc((void *) list, sizeBufferL*sizeof(double*));
				}
				list[indexL].annot = indexA;
				list[indexL].next = start[indexG];
				start[indexG] = indexL;
				indexL++;
			}
			for(; c != EOF && IS_SEP_COMMA(c); c = fgetc(f));
		}
		for(; c!=EOF && issepline(c); c = fgetc(f));
	}
	an->sizeG = sizeG;
	an->sizeA = sizeA;
	start = (int*) realloc((void *) start, an->sizeG*sizeof(int));
	an->mat = (int**) malloc( an->sizeG*sizeof(int*));
	for(i=0; i<an->sizeG; i++) {
		int j;
		an->mat[i] = (int*) malloc(an->sizeA*sizeof(int));
		for(j=0; j<an->sizeA; j++)
			an->mat[i][j] = 0;
		for(j=start[i]; j>=0; j=list[j].next)
			an->mat[i][list[j].annot] = 1;
	}
	free((void*) start);
	free((void*) list);
	an->nameG = (char**) malloc(an->sizeG*sizeof(char*));
	fillLexiTree(an->nameG, dictG);
	an->nameA = (char**) malloc(an->sizeA*sizeof(char*));
	fillLexiTree(an->nameA, dictA);
	freeLexiTree(dictG);
	freeLexiTree(dictA);
	return an;
}

TypeAnnotation *filterAnnotation(TypeAnnotation *in, char **list, char *name) {
	TypeAnnotation *an;
	int i;
	TypeLexiTree *dict;
	
	an = (TypeAnnotation*) malloc(sizeof(TypeAnnotation));
	an->sizeG = in->sizeG;
	an->sizeA = 1;
	an->nameG = (char**) malloc(an->sizeG*sizeof(char*));
	for(i=0; i<an->sizeG; i++)
		an->nameG[i] = strdpl(in->nameG[i]);
	an->nameA = (char**) malloc(sizeof(char*));
	an->nameA[0] = strdpl(name);
	dict = newLexiTree();
	for(i=0; list[i] != NULL; i++)
		if(addWordLexi(list[i], i, dict) != END_INT)
			fprintf(stderr, "Warning! duplicate ident '%s' in list\n", list[i]);
	an->mat = (int**) malloc(an->sizeG*sizeof(int*));
	for(i=0; i<an->sizeG; i++) {
		int j;
		an->mat[i] = (int*) malloc(sizeof(int));
		for(j=0; j<in->sizeA && (in->mat[i][j] == 0 || findWordLexi(in->nameA[j], dict) == END_INT); j++)
			;
		if(j<in->sizeA)
			an->mat[i][0] = 1;
		else
			an->mat[i][0] = 0;
	}
	return an;
}

void fprintAnnotation(FILE *f, TypeAnnotation *an) {
	int i, j;
	if(an == NULL)
		return;
	for(i=0; i<an->sizeG; i++) {
		fprintf(f, "%s", an->nameG[i]);
		for(j=0; j<an->sizeA; j++)
			if(an->mat[i][j])
				fprintf(f, "\t%s", an->nameA[j]);
		fprintf(f, "\n");
	}
}
	
void fprintAnnotationInfo(FILE *f, TypeAnnotation *an) {
	fprintf(f, "%d genes %d annotations\n", an->sizeG, an->sizeA);
}

void freeAnnotation(TypeAnnotation *an) {
	int i, j;
	if(an != NULL) {
		if(an->nameG != NULL) {
			for(i=0; i<an->sizeG; i++) {
				if(an->nameG[i] != NULL)
					free((void*) an->nameG[i]);
			}
			free((void*) an->nameG);
		}
		if(an->mat != NULL) {
			for(i=0; i<an->sizeG; i++) {
				if(an->mat[i] != NULL)
					free((void*) an->mat[i]);
			}
			free((void*) an->mat);
		}
		if(an->nameA != NULL) {
			for(j=0; j<an->sizeA; j++)
				if(an->nameA[j] != NULL)
					free((void*) an->nameA[j]);
			free((void*) an->nameA);
		}
		free((void*) an);
	}
}

TypeOntologyInfo *readOntologyInfo(FILE *f) {
	char c, id[MAX_NAME_SIZE+1], name[MAX_NAME_SIZE+1];
	int sizeBuffer, i;
	TypeOntologyInfo *info;

	info = (TypeOntologyInfo*) malloc(sizeof(TypeOntologyInfo));

	sizeBuffer = BASIC_INC_BUFFER_L;
	info->id = (char**) malloc(sizeBuffer*sizeof(char*));
	info->name = (char**) malloc(sizeBuffer*sizeof(char*));
	info->size = 0;
	for(c=getc(f); c!=EOF && issepline(c); c = fgetc(f));
	while(c != EOF) {
		for(i=0; i<MAX_NAME_SIZE && c !=EOF && !issepline(c); i++) {
			id[i] = c;
			c = fgetc(f);
		}
		id[i] = '\0';
		for(; c!=EOF && issep(c); c = fgetc(f));
		for(i=0; i<MAX_NAME_SIZE && c !=EOF && !isline(c); i++) {
			name[i] = c;
			c = fgetc(f);
		}
		name[i] = '\0';
		if(id[0] != '\0' && name[0] != '\0') {
			if(info->size >= sizeBuffer) {
				sizeBuffer += BASIC_INC_BUFFER_L;
				info->id = (char**) realloc(info->id, sizeBuffer*sizeof(char*));
				info->name = (char**) realloc(info->name, sizeBuffer*sizeof(char*));
			}
			info->id[info->size] = strdpl(id);
			info->name[info->size] = strdpl(name);
			info->size++;
		}
		for(; c!=EOF && issepline(c); c = fgetc(f));
	}
	info->id = (char**) realloc(info->id, info->size*sizeof(char*));
	info->name = (char**) realloc(info->name, info->size*sizeof(char*));
	return info;
}

TypeOntologyInfo *readOntologyInfoBis(FILE *f) {
	char c, first[MAX_NAME_SIZE+1], id[MAX_NAME_SIZE+1], name[MAX_NAME_SIZE+1];
	int sizeBuffer, i;
	TypeOntologyInfo *info;

	info = (TypeOntologyInfo*) malloc(sizeof(TypeOntologyInfo));

	sizeBuffer = BASIC_INC_BUFFER_L;
	info->id = (char**) malloc(sizeBuffer*sizeof(char*));
	info->name = (char**) malloc(sizeBuffer*sizeof(char*));
	info->size = 0;
	for(c=getc(f); c!=EOF && issepline(c); c = fgetc(f));
	do {
		for(i=0; i<MAX_NAME_SIZE && c !=EOF && !issepline(c); i++) {
			first[i] = c;
			c = fgetc(f);
		}
		first[i] = '\0';
		for(; c!=EOF && isline(c); c = fgetc(f));
		for(; c!=EOF && issepline(c); c = fgetc(f));
//printf("first %s\n", first);
	} while(strcmp(first, "[Term]") != 0);
	while(c != EOF) {
		id[0] = '\0';
		name[0] = '\0';
		do {
			for(i=0; i<MAX_NAME_SIZE && c !=EOF && !issepline(c); i++) {
				first[i] = c;
				c = fgetc(f);
			}
			first[i] = '\0';
			if(strcmp(first, "id:") == 0) {
				for(; c!=EOF && issep(c); c = fgetc(f));
				for(i=0; i<MAX_NAME_SIZE && c !=EOF && !isline(c); i++) {
					id[i] = c;
					c = fgetc(f);
				}
				id[i] = '\0';
			}
			if(strcmp(first, "name:") == 0) {
				for(; c!=EOF && issep(c); c = fgetc(f));
				for(i=0; i<MAX_NAME_SIZE && c !=EOF && !isline(c); i++) {
					name[i] = c;
					c = fgetc(f);
				}
				name[i] = '\0';
			}
			for(; c!=EOF && !isline(c); c = fgetc(f));
			for(; c!=EOF && issepline(c); c = fgetc(f));
		} while(c != EOF && strcmp(first, "[Term]") != 0);
		if(id[0] != '\0' && name[0] != '\0') {
			if(info->size >= sizeBuffer) {
				sizeBuffer += BASIC_INC_BUFFER_L;
				info->id = (char**) realloc(info->id, sizeBuffer*sizeof(char*));
				info->name = (char**) realloc(info->name, sizeBuffer*sizeof(char*));
			}
			info->id[info->size] = strdpl(id);
			info->name[info->size] = strdpl(name);
			info->size++;
		}
	}
	info->id = (char**) realloc(info->id, info->size*sizeof(char*));
	info->name = (char**) realloc(info->name, info->size*sizeof(char*));
	return info;
}	

void freeOntologyInfo(TypeOntologyInfo *info) {
	int i;
	if(info == NULL)
		return;
	if(info->name != NULL) {
		for(i=0; i<info->size; i++)
			if(info->name[i] != NULL)
				free((void*)info->name[i]);
		free((void*)info->name);
	}
	if(info->id != NULL) {
		for(i=0; i<info->size; i++)
			if(info->id[i] != NULL)
				free((void*)info->id[i]);
		free((void*)info->id);
	}
	free((void*)info);
}

void fprintOntologyInfo(FILE *f, TypeOntologyInfo *info) {
	int i;
	if(info == NULL)
		return;
	for(i=0; i<info->size; i++) {
		if(info->id[i] != NULL) {
			fprintf(f, "%s:", info->id[i]);
			if(info->name[i] != NULL)
	 			fprintf(f, "%s", info->name[i]);
			fprintf(f, "\n");
		}
	}
}

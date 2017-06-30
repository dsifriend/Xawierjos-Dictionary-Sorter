#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAXLEN 128                                                  /* assumes words won't take more than 128 char */

const char alphabet[] = "aābgdeēzhθiījklmnoōprstuūwyȳφx";
                                                                    /* custom alphabet for sorting */
int abcSize = (int) sizeof(alphabet)/sizeof(alphabet[0]);           /* length of alphabet */

//int lexComp(const void *word1, const void *word2, int pairCount);   /* recursive dictionary sort with custom alphabet */
//int lexComp(const void *word1, const void *word2, int pairCount) {
//    char testWord1[MAXLEN];
//    char testWord2[MAXLEN];
//    strcpy(testWord1, (char *) word1);
//    strcpy(testWord2, (char *) word2);
//
//    int testChar1 = -1;                                             /* # in alphabet of char compared */
//    int testChar2 = -1;                                             /* # in alphabet of char compared */
//    for (int i = 0; i < abcSize; ++i) {                             /* iterates over alphabet */
//        if (testWord1[pairCount] == alphabet[i]) {                  /* checks if alphabet[i] and testWord2 equal */
//            testChar1 = i;                                          /* sets its char comparison value to i */
//        }
//        if (testWord2[pairCount] == alphabet[i]) {                  /* checks if alphabet[i] and testWord2 equal */
//            testChar2 = i;                                          /* sets its char comparison value to i */
//        }
//    }
//
//    if (testChar1 < testChar2) {
//        return -1;                                                  /* Word1 comes before Word2 */
//    }
//    else if (testChar1 > testChar2) {
//        return 1;                                                   /* Word1 comes after Word2 */
//    }
//    else if (testChar1 == -1 && testChar2 == -1) {
//        return 0;                                                   /* end of both words and no difference, same word */
//    }
//    else {                                                          /* ambiguous case, must go deeper */
//        return lexComp(testWord1, testWord2, ++pairCount);          /* does so with recursion recursion */
//    }
//}
//
//int qsortComp(const void *word1, const void *word2);                /* interfaces recursive lexComp w/ qsort */
//int qsortComp(const void *word1, const void *word2) {
//    int pairCount = 0;                                              /* counter to track of letter pair being compared */
//    return lexComp(word1, word2, pairCount);
//}

int lexComp2(const void *word1, const void *word2);                 /* non-recursive sort with custom alphabet */
int lexComp2(const void *word1, const void *word2) {
    char testWord1[MAXLEN];
    char testWord2[MAXLEN];
    strcpy(testWord1, (char *) word1);
    strcpy(testWord2, (char *) word2);

    int testChar1 = -1;                                             /* # in alphabet of char compared */
    int testChar2 = -1;                                             /* # in alphabet of char compared */
    for (int pairCount = 0;
         (testWord1[pairCount] != '\0' || testWord2[pairCount] != '\0');
         ++pairCount) {                                             /* checks whether each pair of letters is \0 */
        for (int i = 0; i < abcSize; ++i) {                         /* iterates over alphabet */
            if (testWord1[pairCount] == alphabet[i]) {              /* checks if alphabet[i] and testWord2 equal */
                testChar1 = i;                                      /* sets its char comparison value to i */
            }
            if (testWord2[pairCount] == alphabet[i]) {              /* checks if alphabet[i] and testWord2 equal */
                testChar2 = i;                                      /* sets its char comparison value to i */
            }
        }
        if (testChar1 != testChar2) {                           /* a change occurred and the return is determined */
            goto returnVal;
        }
    }

    returnVal: {
        if (testChar1 < testChar2) {
            return -1;                                              /* Word1 comes before Word2 */
        }
        if (testChar1 > testChar2) {
            return 1;                                               /* Word1 comes after Word2 */
        }
    };

    return 0;                                                   /* values did not change at any point, same word */
}

int main(int argc, char *argv[]) {
    FILE *dictUnsorted = fopen(argv[1], "r");                       /* opens file passed as argument to extract words */
    if (dictUnsorted == NULL) {
        dictUnsorted = fopen("unsorted_entries.txt", "r");          /* fallback to file in program source-directory */
    }
    FILE *dictSorted = fopen("dictionary_entries.txt", "w+");       /* opens/creates file to sort entries into */

    int tempChar = 0;                                               /* temporary char to store input data into */
    int vocabularySize = 1;                                         /* count for number of words in dictionary */
    bool trackWord = true;                                          /* tracks whether characters should be copied */
    while (tempChar != EOF) {                                       /* checks for EOF to stop process */
        tempChar = fgetc(dictUnsorted);                             /* stores current input data in tempChar */
        if (trackWord) {                                            /* checks whether to copy input */
            if (tempChar == ' ' || tempChar == ',') {               /* checks for end of word in unsorted list */
                fprintf(dictSorted, "\0");                          /* ends word string in output */
                ++vocabularySize;                                   /* increments vocabularySize counter */
                trackWord = false;                                  /* stops tracking */
            }
            else {                                                  /* copies other input directly if still tracking */
                fprintf(dictSorted, "%c", tempChar);
            }
        }
        else {
            if (tempChar == '\n') {                                 /* checks if at end of line for previous entry */
                fprintf(dictSorted, "\n");                          /* inserts newline for next entry */
                trackWord = true;                                   /* begins tracking again */
            }
        }
    }

    fseek(dictSorted, 0, SEEK_SET);                                 /* goes back to start of output file */

    tempChar = 0;                                                   /* resets tempChar */
    char dictEntries[vocabularySize][MAXLEN];                       /* array to store entries into for sorting */
    int wordNum = 0;                                                /* counter for word being added to entries array */
    while (tempChar != EOF) {                                       /* checks for EOF to stop process */
        fscanf(dictSorted, "%s", dictEntries[wordNum++]);           /* copies string before incrementing wordNum */
        tempChar = fgetc(dictSorted);                               /* moves to next data chunk in file, stores input */
    }

    fseek(dictSorted, 0, SEEK_SET);                                 /* goes back to start of output file */

    qsort(dictEntries, (size_t) vocabularySize, MAXLEN*sizeof(char), lexComp2);
                                                                    /* sorts that shit */

    for (int i = 0; i < vocabularySize; ++i) {
        fprintf(dictSorted,"%s\n", dictEntries[i]);
    }

    fclose(dictUnsorted); fclose(dictSorted);                       /* closes files */

    return 0;
}
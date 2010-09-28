/*
 * Copyright (c) 2009 Openmoko Inc.
 *
 * Authors   Holger Hans Peter Freyther <zecke@openmoko.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grifo.h"
#include "languages.h"
#include "mapping_tables.h"
#include "utf8.h"
#include "bigram.h"
#include "search.h"
#include "wikilib.h"

char *get_hiragana(char *in_str, int len, int *used_len)
{
	int i;
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(english_hiragana_mapping) / sizeof(struct _english_hiragana_mapping) - 1;
	int iMiddle = 0;
	char *pReturn = NULL;

	while (!bFound && iStart <= iEnd)
	{
		iMiddle = (iStart + iEnd) / 2;
		if (*in_str == english_hiragana_mapping[iMiddle].english[0])
			bFound = 1;
		else if (*in_str > english_hiragana_mapping[iMiddle].english[0])
		{
            if (iMiddle == iStart)
                iStart++;
            else
                iStart = iMiddle;
		}
		else
		{
            if (iMiddle == iEnd)
                iEnd--;
            else
                iEnd = iMiddle;
        }
	}

	if (bFound) // find the first hiragana_mapping entry with the same starting character as in_str
	{
		while (iMiddle > 0 && *in_str == english_hiragana_mapping[iMiddle - 1].english[0])
			iMiddle--;

		for (i = iMiddle; i < sizeof(english_hiragana_mapping) / sizeof(struct _english_hiragana_mapping) && *in_str == english_hiragana_mapping[i].english[0]; i++)
		{
			if (len >= strlen(english_hiragana_mapping[i].english) && !strncmp(in_str, english_hiragana_mapping[i].english, strlen(english_hiragana_mapping[i].english)))
			{
				*used_len = strlen(english_hiragana_mapping[i].english);
				pReturn = english_hiragana_mapping[i].hiragana;
			}
		}
	}

	return pReturn;
}

char *get_english(char *in_str, int len, int *used_len)
{
	int i;
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(zh_jp_english_mapping) / sizeof(struct _zh_jp_english_mapping) - 1;
	int iMiddle = 0;
	char *pReturn = NULL;
	char first_utf8_char[5];
	int cmp;
	int len_first_char;

	get_first_utf8_char(first_utf8_char, in_str, strlen(in_str));
	len_first_char = strlen(first_utf8_char);
	*used_len = len_first_char;
	while (!bFound && iStart <= iEnd)
	{
		iMiddle = (iStart + iEnd) / 2;
		cmp = strncmp(first_utf8_char, zh_jp_english_mapping[iMiddle].hiragana, len_first_char);
		if (!cmp)
			bFound = 1;
		else if (cmp > 0)
		{
            if (iMiddle == iStart)
                iStart++;
            else
                iStart = iMiddle;
		}
		else
		{
            if (iMiddle == iEnd)
                iEnd--;
            else
                iEnd = iMiddle;
        }
	}

	if (bFound) // find the first hiragana_mapping entry with the same starting character as in_str
	{
		while (iMiddle > 0 && !strncmp(first_utf8_char, zh_jp_english_mapping[iMiddle - 1].hiragana, len_first_char))
			iMiddle--;

		for (i = iMiddle; i < sizeof(zh_jp_english_mapping) / sizeof(struct _zh_jp_english_mapping) &&
			!strncmp(first_utf8_char, zh_jp_english_mapping[i].hiragana, len_first_char); i++)
		{
			if (len >= strlen(zh_jp_english_mapping[i].hiragana) && !strncmp(in_str, zh_jp_english_mapping[i].hiragana, strlen(zh_jp_english_mapping[i].hiragana)))
			{
				*used_len = strlen(zh_jp_english_mapping[i].hiragana);
				pReturn = zh_jp_english_mapping[i].english;
			}
		}
	}

	return pReturn;
}

void hiragana_romaji_conversion(char *search_string_per_language, int *search_str_per_language_len)
{
	int conversion_start_pos;
	char temp_hiragana[MAX_TITLE_SEARCH * 3];
	int temp_hiragana_len = 0;
	int i;
	char *pHiragana = NULL;
	int used_len;
	int iFirstRomaji = -1;
	int iBeforeLastHiragana = -1;

	if (*search_str_per_language_len > 0)
	{
		conversion_start_pos = *search_str_per_language_len;
		while (conversion_start_pos > 0)
		{
			if (search_string_per_language[conversion_start_pos - 1] & 0x80) // no more conversion beyond utf8 character
				break;
			conversion_start_pos--;
		}

		i = 0;
		while (i < *search_str_per_language_len)
		{
			if (i < conversion_start_pos)
			{
				char *pFull;

				if (!(search_string_per_language[i] & 0x80) && (pFull = half_alphabet_to_full(search_string_per_language[i])))
				{
					strcpy(&temp_hiragana[temp_hiragana_len], pFull);
					temp_hiragana_len += strlen(pFull);
					i++;
				}
				else
					temp_hiragana[temp_hiragana_len++] = search_string_per_language[i++];
			}
			else
			{
				pHiragana = get_hiragana(&search_string_per_language[i], *search_str_per_language_len - i, &used_len);
				if (pHiragana)
				{
					iBeforeLastHiragana = temp_hiragana_len;
					memcpy(&temp_hiragana[temp_hiragana_len], pHiragana, strlen(pHiragana));
					temp_hiragana_len += strlen(pHiragana);
					i += used_len;
				}
				else
				{
					if (iFirstRomaji < 0 && 'a' <= search_string_per_language[i] && search_string_per_language[i] <= 'z')
						iFirstRomaji = temp_hiragana_len;
					temp_hiragana[temp_hiragana_len++] = search_string_per_language[i++];
				}
			}
		}


		memcpy(search_string_per_language, temp_hiragana, temp_hiragana_len);
		*search_str_per_language_len = temp_hiragana_len;
		search_string_per_language[*search_str_per_language_len] = '\0';

		if (iFirstRomaji >= 0 && iFirstRomaji < iBeforeLastHiragana)
		{
			if (iFirstRomaji > 0)
			{
				memcpy(temp_hiragana, search_string_per_language, iFirstRomaji);
				temp_hiragana_len = iFirstRomaji;
			}
			else
				temp_hiragana_len = 0;
			i = iFirstRomaji;
			while (i < iBeforeLastHiragana)
			{
				char *pFull;

				if (!(search_string_per_language[i] & 0x80) && (pFull = half_alphabet_to_full(search_string_per_language[i])))
				{
					strcpy(&temp_hiragana[temp_hiragana_len], pFull);
					temp_hiragana_len += strlen(pFull);
					i++;
				}
				else
					temp_hiragana[temp_hiragana_len++] = search_string_per_language[i++];
			}
			memcpy(&temp_hiragana[temp_hiragana_len], &search_string_per_language[iBeforeLastHiragana], *search_str_per_language_len - iBeforeLastHiragana);
			temp_hiragana_len += *search_str_per_language_len - iBeforeLastHiragana;

			memcpy(search_string_per_language, temp_hiragana, temp_hiragana_len);
			*search_str_per_language_len = temp_hiragana_len;
			search_string_per_language[*search_str_per_language_len] = '\0';
		}
	}
}

int replace_japanese_sonant(char *search_string_per_language, int *search_str_per_language_len, char *search_string, int *search_str_len)
{
	char last_utf8_char[5];
	int rc = -1;
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(sonant_conversion) / sizeof(struct _sonant_conversion) - 1;
	int iMiddle = 0;
	int cmp;

	if (*search_str_per_language_len > 0)
	{
		get_last_utf8_char(last_utf8_char, search_string_per_language, *search_str_per_language_len);
		while (!bFound && iStart <= iEnd)
		{
			iMiddle = (iStart + iEnd) / 2;
			cmp = strcmp(last_utf8_char, sonant_conversion[iMiddle].origin);
			if (!cmp)
				bFound = 1;
			else if (cmp > 0)
			{
                if (iMiddle == iStart)
                    iStart++;
                else
                    iStart = iMiddle;
			}
			else
			{
                if (iMiddle == iEnd)
                    iEnd--;
                else
                    iEnd = iMiddle;
            }
		}

		if (bFound)
		{
			memcpy(&search_string_per_language[*search_str_per_language_len - strlen(last_utf8_char)], sonant_conversion[iMiddle].after_conversion, strlen(sonant_conversion[iMiddle].after_conversion));
			*search_str_per_language_len += strlen(last_utf8_char) - strlen(sonant_conversion[iMiddle].after_conversion);
			search_string_per_language[*search_str_per_language_len] = '\0';
			*search_str_len = zh_jp_to_english(search_string, MAX_TITLE_SEARCH, search_string_per_language, search_str_per_language_len);
			rc = 0;
		}
	}
 	return rc;
}

int replace_hiragana_backward(char *search_string_per_language, int *search_str_per_language_len, char *search_string, int *search_str_len)
{
	char last_utf8_char[5];
	int rc = -1;
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(backward_key_sequence) / sizeof(struct _backward_key_sequence) - 1;
	int iMiddle = 0;
	int cmp;

	if (*search_str_per_language_len > 0)
	{
		get_last_utf8_char(last_utf8_char, search_string_per_language, *search_str_per_language_len);
		while (!bFound && iStart <= iEnd)
		{
			iMiddle = (iStart + iEnd) / 2;
			cmp = strcmp(last_utf8_char, backward_key_sequence[iMiddle].origin);
			if (!cmp)
				bFound = 1;
			else if (cmp > 0)
			{
                if (iMiddle == iStart)
                    iStart++;
                else
                    iStart = iMiddle;
			}
			else
			{
                if (iMiddle == iEnd)
                    iEnd--;
                else
                    iEnd = iMiddle;
            }
		}

		if (bFound)
		{
			memcpy(&search_string_per_language[*search_str_per_language_len - strlen(last_utf8_char)], backward_key_sequence[iMiddle].after_conversion, strlen(backward_key_sequence[iMiddle].after_conversion));
			*search_str_per_language_len += strlen(backward_key_sequence[iMiddle].after_conversion) - strlen(last_utf8_char);
			search_string_per_language[*search_str_per_language_len] = '\0';
			hiragana_romaji_conversion(search_string_per_language, search_str_per_language_len);
			*search_str_len = zh_jp_to_english(search_string, MAX_TITLE_SEARCH, search_string_per_language, search_str_per_language_len);
			rc = 0;
		}
	}
	return rc;
}

int zh_jp_to_english(char *sEnglish, int maxLenEnglish, char *sHiragana, int *lenHiragana)
{
	int lenEnglish = 0;
	int i = 0;
	char *pEnglish;
	int used_len;

	while (i < *lenHiragana && lenEnglish < maxLenEnglish - 1)
	{
		pEnglish = get_english(&sHiragana[i], *lenHiragana - i, &used_len);
		if (pEnglish)
		{
			if (lenEnglish < maxLenEnglish - strlen(pEnglish))
			{
				memcpy(&sEnglish[lenEnglish], pEnglish, strlen(pEnglish));
				lenEnglish += strlen(pEnglish);
				i += used_len;
			}
			else
			{
				sEnglish[lenEnglish] = '\0';
				*lenHiragana = i;
				sHiragana[*lenHiragana] = '\0';
				handle_search_key(NULL, 0); // reset the key change
				return lenEnglish;
			}
		}
		else if (used_len == 1 && is_supported_search_char(sHiragana[i])) // used_len contains the length of the first UTF8 char
		{
			sEnglish[lenEnglish++] = sHiragana[i++];
		}
		else
			i += used_len;
	}
	sEnglish[lenEnglish] = '\0';
	if (i < *lenHiragana)
	{
		*lenHiragana = i;
		sHiragana[*lenHiragana] = '\0';
		handle_search_key(NULL, 0); // reset the key change
	}
	return lenEnglish;
}

void alphabet_to_korean_jamo(char *jamo, char eng)
{
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(korean_jamo) / sizeof(struct _korean_jamo) - 1;
	int iMiddle = 0;

	while (!bFound && iStart <= iEnd)
	{
		iMiddle = (iStart + iEnd) / 2;
		if (eng == *korean_jamo[iMiddle].english)
			bFound = 1;
		else if (eng > *korean_jamo[iMiddle].english)
		{
            if (iMiddle == iStart)
                iStart++;
            else
                iStart = iMiddle;
		}
		else
		{
            if (iMiddle == iEnd)
                iEnd--;
            else
                iEnd = iMiddle;
        }
	}

	if (bFound)
		strcpy(jamo, korean_jamo[iMiddle].jamo);
	else
        *jamo = '\0';
}

int jamo_index(int state, char *in_str, int in_len, int *used_len)
{
	int bFound = 0;
	int iStart = 0;
	int iEnd = sizeof(korean_jamo_ex) / sizeof(struct _korean_jamo_ex) - 1;
	int iMiddle = 0;
	int rc = -1;
	int i;

	*used_len = 0;
	in_str[in_len] = '\0'; // make sure it's null terminated
	while (!bFound && iStart <= iEnd)
	{
		iMiddle = (iStart + iEnd) / 2;
		if (*in_str == *korean_jamo_ex[iMiddle].english)
			bFound = 1;
		else if (*in_str > *korean_jamo_ex[iMiddle].english)
		{
            if (iMiddle == iStart)
                iStart++;
            else
                iStart = iMiddle;
		}
		else
		{
            if (iMiddle == iEnd)
                iEnd--;
            else
                iEnd = iMiddle;
        }
	}

	if (bFound)
	{
		// find the first entry with the same starting character as in_str
		while (iMiddle > 0 && *in_str == korean_jamo_ex[iMiddle - 1].english[0])
			iMiddle--;
    	
		bFound = 0; // found the expected jamo category (initial, medial or final)
		for (i = iMiddle; i < sizeof(korean_jamo_ex) / sizeof(struct _korean_jamo_ex) && *in_str == korean_jamo_ex[i].english[0]; i++)
		{
			if (in_len >= strlen(korean_jamo_ex[i].english) && !strncmp(in_str, korean_jamo_ex[i].english, strlen(korean_jamo_ex[i].english)))
			{
				if (!bFound || // the expected jamo category has the higher priority
					(state == STATE_INITIAL && (korean_jamo_ex[i].jamo_idx & INITIAL_JAMO_BASE)) ||
					(state == STATE_AFTER_INITIAL_JAMO && (korean_jamo_ex[i].jamo_idx & MEDIAL_JAMO_BASE)) ||
					(state == STATE_AFTER_MEDIAL_JAMO && (korean_jamo_ex[i].jamo_idx & FINAL_JAMO_BASE)))
				{
			        rc = i;
			        *used_len = strlen(korean_jamo_ex[i].english);
			        if ((state == STATE_INITIAL && (korean_jamo_ex[i].jamo_idx & INITIAL_JAMO_BASE)) ||
						(state == STATE_AFTER_INITIAL_JAMO && (korean_jamo_ex[i].jamo_idx & MEDIAL_JAMO_BASE)) ||
						(state == STATE_AFTER_MEDIAL_JAMO && (korean_jamo_ex[i].jamo_idx & FINAL_JAMO_BASE)))
			        	bFound = 1;
			    }
			}
		}
	}
	
	return rc;
}

int parse_english_for_jamo(int *iaJamo, int *iaUsedLen, char *in_str, int in_len)
{
    int parsed_len = 0;
    int nJamoCount = 0;
    int state = STATE_INITIAL;
    int jamo_ex_idx;
    int used_len;

    while (parsed_len < in_len && nJamoCount < MAX_TITLE_SEARCH)
    {
        if ((unsigned char)in_str[parsed_len] == 0xEF && (unsigned char)in_str[parsed_len + 1] == 0xBD &&
            0x81 <= (unsigned char)in_str[parsed_len + 2] && (unsigned char)in_str[parsed_len + 2] <= 0x9A) // full-width alphabet is for Englsih
    	{
    		iaUsedLen[nJamoCount] = parsed_len + 3;
    		iaJamo[nJamoCount++] = 'a' + ((unsigned char)in_str[parsed_len +2] - 0x81);
    		parsed_len += 3;
    		state = STATE_INITIAL;
    	}
    	else
    	{
            jamo_ex_idx = jamo_index(state, &in_str[parsed_len], in_len - parsed_len, &used_len);
            if (jamo_ex_idx >= 0)
            {
            	switch (state)
            	{
                    case STATE_INITIAL:
                        if ((korean_jamo_ex[jamo_ex_idx].jamo_idx & MEDIAL_JAMO_BASE) && 
                        	nJamoCount > 0 && (iaJamo[nJamoCount - 1] & JAMO_INDEX_BASE) &&
                        	(korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx & FINAL_JAMO_BASE))
                        {
                        	if (strlen(korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].english) == 2)
	                    	{
	                    		char sTemp[3];
	                    		int nTempUsed;
	                    		int nTempIdx1;
	                    		int nTempIdx2;
	                    		sTemp[0] = korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].english[0];
	                    		sTemp[1] = '\0';
	                    		nTempIdx1 = jamo_index(STATE_AFTER_MEDIAL_JAMO, sTemp, 1, &nTempUsed);
	                    		sTemp[0] = korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].english[1];
	                    		nTempIdx2 = jamo_index(STATE_INITIAL, sTemp, 1, &nTempUsed);
	                    		if (nTempIdx1 >= 0 && nTempIdx2 >= 0)
	                    		{ // split the double keys to "final" and initial jamos
    								iaUsedLen[nJamoCount - 1]--;
		                    		iaJamo[nJamoCount - 1] = JAMO_INDEX_BASE | nTempIdx1;
		                    		iaUsedLen[nJamoCount] = iaUsedLen[nJamoCount - 1] + 1;
	                    			iaJamo[nJamoCount++] = JAMO_INDEX_BASE | nTempIdx2;
	                    			state = STATE_AFTER_MEDIAL_JAMO;
	                    		}
	                    		else
	                    		{
		                    		sTemp[0] = korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].english[0];
		                    		sTemp[1] = korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].english[1];
		                    		sTemp[2] = '\0';
	                    			nTempIdx1 = jamo_index(STATE_INITIAL, sTemp, 2, &nTempUsed);
	                    			if (nTempIdx1 >= 0 && (korean_jamo_ex[nTempIdx1].jamo_idx & INITIAL_JAMO_BASE))
	                    			{
	                    				iaJamo[nJamoCount - 1] = JAMO_INDEX_BASE | nTempIdx1;
	                    				state = STATE_AFTER_MEDIAL_JAMO;
	                    			}
	                    			else
	                    				state = STATE_INITIAL;
	                    		}
	                    		parsed_len += used_len;
    							iaUsedLen[nJamoCount] = parsed_len;
	                    		iaJamo[nJamoCount++] = JAMO_INDEX_BASE | jamo_ex_idx;
	                    	}
	                    	else
	                    	{
	                    		char sTemp[3];
	                    		int nTempUsed;
	                    		int nTempIdx1;
	                    		sTemp[0] = korean_jamo_ex[iaJamo[nJamoCount - 1] & JAMO_INDEX_EXCLUDE_BASE].english[0];
	                    		sTemp[1] = '\0';
	                    		nTempIdx1 = jamo_index(STATE_INITIAL, sTemp, 1, &nTempUsed);
	                    		if (nTempIdx1 >= 0 && (korean_jamo_ex[nTempIdx1].jamo_idx & INITIAL_JAMO_BASE))
	                    		{
	                    			iaJamo[nJamoCount - 1] = JAMO_INDEX_BASE | nTempIdx1;
	                    				state = STATE_AFTER_MEDIAL_JAMO;
	                    		}
	                    		else
	                    			state = STATE_INITIAL;
	                    		parsed_len += used_len;
    							iaUsedLen[nJamoCount] = parsed_len;
	                    		iaJamo[nJamoCount++] = JAMO_INDEX_BASE | jamo_ex_idx;
	                    	}
                        }
                        else
                        {
			            	parsed_len += used_len;
    						iaUsedLen[nJamoCount] = parsed_len;
			            	iaJamo[nJamoCount++] = JAMO_INDEX_BASE | jamo_ex_idx;
                        	if (korean_jamo_ex[jamo_ex_idx].jamo_idx & INITIAL_JAMO_BASE)
			            		state = STATE_AFTER_INITIAL_JAMO;
                        	else
			            		state = STATE_INITIAL;
                        } 
                        break;
                    case STATE_AFTER_INITIAL_JAMO:
			            parsed_len += used_len;
    					iaUsedLen[nJamoCount] = parsed_len;
 			            iaJamo[nJamoCount++] = JAMO_INDEX_BASE | jamo_ex_idx;
						if (korean_jamo_ex[jamo_ex_idx].jamo_idx & MEDIAL_JAMO_BASE)
							state = STATE_AFTER_MEDIAL_JAMO;
                        else if (korean_jamo_ex[jamo_ex_idx].jamo_idx & INITIAL_JAMO_BASE)
                        	state = STATE_AFTER_INITIAL_JAMO;
						else
							state = STATE_INITIAL;
                        break;
                    case STATE_AFTER_MEDIAL_JAMO:
			            parsed_len += used_len;
    					iaUsedLen[nJamoCount] = parsed_len;
 			            iaJamo[nJamoCount++] = JAMO_INDEX_BASE | jamo_ex_idx;
                        if (korean_jamo_ex[jamo_ex_idx].jamo_idx & INITIAL_JAMO_BASE)
                        	state = STATE_AFTER_INITIAL_JAMO;
						else
							state = STATE_INITIAL;
                        break;
                    default:
                        break;
                }
            }
            else
            {
	    		iaUsedLen[nJamoCount] = parsed_len + 1;
            	iaJamo[nJamoCount++] = in_str[parsed_len];
            	parsed_len++;
                state = STATE_INITIAL;
            }
    	}
    }
    
    return nJamoCount;
}

int english_to_korean(char *out_str, int max_out_len, char *in_str, int *in_len)
{
    int iaJamo[MAX_TITLE_SEARCH];
    int iaUsedLen[MAX_TITLE_SEARCH];
    int nJamoCount;
    int i;
    int out_len;
    
    if (!(*in_len))
    {
	    out_str[0] = '\0';
    	return 0;
    }
    	
    nJamoCount = parse_english_for_jamo(iaJamo, iaUsedLen, in_str, *in_len);
    // note: parse_english_for_jamo will be determin the proper initial, medial and final jamo according to the context
    
    i = 0;
    out_len = 0;
    while (i < nJamoCount && out_len < max_out_len)
    {
    	if (iaJamo[i] & JAMO_INDEX_BASE)
    	{
    		int nIdxInitialJamo = -1;
    		int nIdxMedialJamo = -1;
    		int nIdxFinalJamo = -1;
    		
    		nIdxInitialJamo = korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx;
    		if (nIdxInitialJamo & INITIAL_JAMO_BASE)
    		{
    			if (i < nJamoCount - 1 && iaJamo[i + 1] & JAMO_INDEX_BASE) 
    			{
    				nIdxMedialJamo = korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx;
    				if (nIdxMedialJamo & MEDIAL_JAMO_BASE)
    				{
    					if (i < nJamoCount - 2 && iaJamo[i + 2] & JAMO_INDEX_BASE)
    						nIdxFinalJamo = korean_jamo_ex[iaJamo[i + 2] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx;
    				}
    			}
    		}
    		if ((nIdxInitialJamo  & INITIAL_JAMO_BASE) && nIdxMedialJamo != -1 && (nIdxMedialJamo &  MEDIAL_JAMO_BASE))
    		{
    			ucs4_t u = 0xAC00;
    			char utf8[5];
    			
    			u += (nIdxInitialJamo & JAMO_INDEX_EXCLUDE_BASE) * MEDIAL_JAMO_COUNT * FINAL_JAMO_COUNT +
    				(nIdxMedialJamo & JAMO_INDEX_EXCLUDE_BASE) * FINAL_JAMO_COUNT;
    			if (nIdxFinalJamo != -1 && (nIdxFinalJamo &  FINAL_JAMO_BASE))
    			{
    				u += (nIdxFinalJamo & JAMO_INDEX_EXCLUDE_BASE);
    				UCS4_to_UTF8(u, utf8);
    				if (out_len + strlen(utf8) < max_out_len)
    				{
	    				strcpy(&out_str[out_len], utf8);
	    				out_len += strlen(utf8);
    					i += 3;
    					continue;
    				}
    				else
    					u -= (nIdxFinalJamo & JAMO_INDEX_EXCLUDE_BASE);
    			}
    			
    			UCS4_to_UTF8(u, utf8);
    			if (out_len + strlen(utf8) < max_out_len)
    			{
	    			strcpy(&out_str[out_len], utf8);
	    			out_len += strlen(utf8);
    				i += 2;
    				continue;
    			}
    		}
    		
    		if (out_len + strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].jamo) < max_out_len)
    		{  
	    		strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].jamo);
	    		out_len += strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].jamo);
	    		i++;
	    	}
	    	else
	    	{
	    		*in_len = iaUsedLen[i - 1];
	    		break;
	    	}
    	}
    	else
    	{
    		out_str[out_len++] = iaJamo[i++];
    	}
    }
    out_str[out_len] = '\0';
    return out_len;
}

int english_to_korean_phonetic(char *out_str, int max_out_len, char *in_str, int *in_len)
{
    int iaJamo[MAX_TITLE_SEARCH];
    int iaUsedLen[MAX_TITLE_SEARCH];
    int nJamoCount;
    int i;
    int out_len;
    
    if (!(*in_len))
    {
    	out_str[0] = '\0';
    	return 0;
    }
    	
    nJamoCount = parse_english_for_jamo(iaJamo, iaUsedLen, in_str, *in_len);
    i = 0;
    out_len = 0;
    while (i < nJamoCount && out_len < max_out_len)
    {
    	if (iaJamo[i] & JAMO_INDEX_BASE)
    	{
    		int nIdxInitialJamo = -1;
    		int nIdxMedialJamo = -1;
    		int nIdxFinalJamo = -1;
    		
    		nIdxInitialJamo = korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx;
    		if (nIdxInitialJamo & INITIAL_JAMO_BASE)
    		{
    			if (i < nJamoCount - 1 && iaJamo[i + 1] & JAMO_INDEX_BASE) 
    			{
    				nIdxMedialJamo = korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx;
    				if (nIdxMedialJamo & MEDIAL_JAMO_BASE)
    				{
    					if (i < nJamoCount - 2 && iaJamo[i + 2] & JAMO_INDEX_BASE)
    						nIdxFinalJamo = korean_jamo_ex[iaJamo[i + 2] & JAMO_INDEX_EXCLUDE_BASE].jamo_idx;
    				}
    			}
    		}
    		if ((nIdxInitialJamo  & INITIAL_JAMO_BASE) && nIdxMedialJamo != -1 && (nIdxMedialJamo &  MEDIAL_JAMO_BASE))
    		{
    			if (out_len + strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic) +
    				strlen(korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic) < max_out_len)
    			{
	    			if (nIdxFinalJamo != -1 && (nIdxFinalJamo &  FINAL_JAMO_BASE))
	    			{
	    				if (out_len + strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic) +
	    					strlen(korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic) +
	    					strlen(korean_jamo_ex[iaJamo[i + 2] & JAMO_INDEX_EXCLUDE_BASE].phonetic) < max_out_len)
	    				{
		    				strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    				out_len += strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    				strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    				out_len += strlen(korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    				strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i + 2] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    				out_len += strlen(korean_jamo_ex[iaJamo[i + 2] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
	    					i += 3;
	    					continue;
	    				}
	    			}
	    		}
    			
    			if (out_len + strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic) +
	    			strlen(korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic) < max_out_len)
    			{
		    		strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    		out_len += strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    		strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
		    		out_len += strlen(korean_jamo_ex[iaJamo[i + 1] & JAMO_INDEX_EXCLUDE_BASE].phonetic);
    				i += 2;
    				continue;
    			}
    		}
    		
    		if (out_len + strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic_name) < max_out_len)
    		{  
	    		strcpy(&out_str[out_len], korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic_name);
	    		out_len += strlen(korean_jamo_ex[iaJamo[i] & JAMO_INDEX_EXCLUDE_BASE].phonetic_name);
	    		i++;
	    	}
	    	else
	    	{
	    		*in_len = iaUsedLen[i - 1];
	    		break;
	    	}
    	}
    	else
    	{
    		out_str[out_len++] = iaJamo[i++];
    	}
    }
    out_str[out_len] = '\0';
    return out_len;
}

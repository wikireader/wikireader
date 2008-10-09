/*
 * tst tree 
 * (C) Copyright 2008 OpenMoko, Inc.
 * Author: xiangfu liu <xiangfu@openmoko.org>
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

/* Ternary Search Trees 
 * Dr. Dobb's Journal April 1998 By Jon Bentley and Bob Sedgewick
 * Jon is a Member of Technical Staff at Bell Labs.
 * Bob is the William O. Baker Professor of Computer Science at
 * Princeton University. They can be reached at jlb@research.bell-labs.com
 * and rs@cs.princeton.edu, respectively.
 * 
 * We begin with a recursive version of the search function.
 * It returns 1 if string s is in the subtree rooted at p, and 0 otherwise;
 * it is originally called as rsearch(root, s):
 */

int rsearch(Tptr p, char *s) 
{
	if (!p) return 0; 
	if (*s < p->splitchar) 
		return rsearch(p->lokid, s); 
	else if (*s > p->splitchar) 
		return rsearch(p->hikid, s); 
	else { 
		if (*s == 0) return 1; 
		return rsearch(p->eqkid, ++s); 
	} 
} 

Tptr insert(Tptr p, char *s)
{
	if (p == 0) { 
		p = (Tptr) malloc(sizeof(Tnode)); 
		p->splitchar = *s; 
		p->lokid = p->eqkid = p->hikid = 0; 
	} 
	if (*s < p->splitchar) 
		p->lokid = insert(p->lokid, s); 
	else if (*s == p->splitchar) { 
		if (*s != 0) 
			p->eqkid = insert(p->eqkid, ++s); 
	} else 
		p->hikid = insert(p->hikid, s); 
	return p; 
} 
void traverse(Tptr p) 
{
	if (!p) return; 
	traverse(p->lokid); 
	if (p->splitchar) 
		traverse(p->eqkid); 
	else 
		printf("%s/n", (char *) p->eqkid); 
	traverse(p->hikid); 
} 

/* other version search */
int rsearch2(Tptr p, char *s) 
{
	return (!p ? 0 : ( 
			*s == p->splitchar 
			? (*s ? rsearch2(p->eqkid, ++s) : 1) 
			: (rsearch2(	*s < p->splitchar 
					? p->lokid 
					: p->hikid, s)) 
			)
		); 
} 

int search(char *s) 
{
	Tptr p; 
	p = root; 
	while (p) { 
		if (*s < p->splitchar) 
			p = p->lokid; 
		else if (*s == p->splitchar) { 
			if (*s++ == 0) 
				return 1; 
			p = p->eqkid; 
		} else 
			p = p->hikid; 
	} 
	return 0; 
} 

int hashfunc(char *s) 
{ 
	unsigned n = 0; 
        for ( ; *s; s++) 
		n = 31 * n + *s; 
        return n % tabsize; 
} 

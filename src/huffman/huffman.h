//-------------------------------------------------------------------------------
// huffman.h
//-------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//-------------------------------------------------------------------------------
// Copyright (C) 2002 Sean White, 2007 Brad Carney, 2008 Rivecoder/GhostlyDeath
//-------------------------------------------------------------------------------
//
// Version 4, released 8/1/2008. Compatible with Skulltag launchers and servers.
//
//-------------------------------------------------------------------------------

#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__

struct huffnode_t
{
	public:
		huffnode_t *zero;
		huffnode_t *one;
		unsigned char val;
		float freq;
};

struct hufftab_t
{
	public:
		unsigned int bits;
		int len;
};

class Huffman
{
	public:
		Huffman();
		~Huffman();

		void		encode(const char *in, char *out, int inlen, int *outlen);
		void		decode(const char *in, char *out, int inlen, int *outlen);

	protected:
		void		buildTree(float *freq);
		void		findTab(huffnode_t *tmp, int len, unsigned int bits);
		void		putBit(char *puf, int pos, int bit);
		int			getBit(const char *puf, int pos);
		static void	recursiveFreeNode(huffnode_t *pNode);

		int				lastCompMessageSize;
		huffnode_t		*huffTree;
		hufftab_t		huffLookup[256];
		static float	huffFreq[256];
};

extern Huffman g_Huffman;

#endif // __HUFFMAN_H__

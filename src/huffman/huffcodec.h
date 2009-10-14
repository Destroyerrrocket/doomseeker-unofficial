/**
 * skulltag::HuffmanCodec class - Huffman encoder and decoder.
 * Version 1 - Revision 0
 *
 * Copyright 2009 Timothy Landers
 * email: code.vortexcortex@gmail.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* ***** Changelog: huffcodec.h *****
 * 2009.09.30 - v1 r0
 * 		Intitial Release
 */

#ifndef _HUFFMAN_CODEC_VERSION
#define _HUFFMAN_CODEC_VERSION 1
#define _HUFFMAN_CODEC_REV 0

#include "codec.h"
#include "bitwriter.h"

namespace skulltag {

	/** HuffmanCodec class - Encodes and Decodes data using */
	class HuffmanCodec : public Codec {

		/** top level node of the Huffman tree used for decoding. */
		HuffmanNode * root;

		/** table of Huffman codes and bit lengths used for encoding. */
		HuffmanNode ** codeTable;

		/** stores intermediary huffman codes. */
		BitWriter * codeBuffer;

		/** When true this HuffmanCodec reverses its bytes after encoding and before decoding to
		 * provide compatibility with the backwards bit ordering of the original ST Huffman Encoding.
		 * Default value is "false" (do not reverse bits). */
		bool reverseBits;

		/** When false this HuffmanCodec return -1 instead of expanding data during encoding.
		 * Default value is "true" (allow data expansion). */
		bool expandable;

		/** Reverses the order of bits in a byte.
		 *	EG: The statement <code>reverseMap[0xAF] == 0xF5</code> is <code>true</code>. <br>
		 *	The index <code>10101111</code> stores the reverse value: <code>11110101</code>. <br>
		 *  Note: One array lookup is much faster than Eight bit manipulating loop iterations. */
		static unsigned char const reverseMap[];

	public:

		/** Creates a new HuffmanCodec from the tree
		 * @param treeData 	*/
		HuffmanCodec( unsigned char const * const treeData, int dataLength );

		/** Frees associated resources with this HuffmanCodec. */
		~HuffmanCodec();

		/** Decodes data read from an input buffer and stores the result in the output buffer.
		 * @return number of bytes stored in the output buffer or -1 if an error occurs while encoding. */
		virtual int encode(
			unsigned char const * const input,	/**< in: pointer to the first byte to encode. */
			unsigned char * const output,		/**< out: pointer to an output buffer to store data. */
			int const &inLength,				/**< in: number of bytes of input buffer to encoded. */
			int const &outLength				/**< in: maximum length of data to output. */
		) const;

		/** Decodes data read from an input buffer and stores the result in the output buffer.
		 * @return number of bytes stored in the output buffer or -1 if an error occurs while decoding. */
		virtual int decode(
			unsigned char const * const input,	/**< in: pointer to data that needs decoding. */
			unsigned char * const output,		/**< out: pointer to output buffer to store decoded data. */
			int const &inLength,				/**< in: number of bytes of input buffer to read. */
			int const &outLength				/**< in: maximum length of data to output. */
		);

		/** Enables or Disables backwards bit ordering of bytes.
		 * @param backwards  "true" enables reversed bit order bytes, "false" uses standard byte bit ordering. */
		void reversedBytes( bool backwards );

		/** Check the state of backwards bit ordering for bytes.
		 * @return  true: bits within bytes are reversed. false: bits within bytes are normal. */
		bool reversedBytes();

		/** Enable or Disable data expansion during encoding.
		 * @param expandable	"true" allows encoding to expand data. "false" causes failure upon expansion. */
		void allowExpansion( bool expandable );

		/** Check the state of data expandability.
		 * @return	 true: data expansion is allowed.  false: data is not allowed to expand. */
		bool allowExpansion();

	private:

		/** Deletes a Huffman Tree structure by traversing and deleting its nodes. */
		void deleteTree( HuffmanNode * root );

		/** Recursively builds the Huffman Tree. */
		int buildTree( HuffmanNode * node, unsigned char const treeData[], int index, int dataLength );

	}; // end class Huffman Codec.
} // end namespace skulltag

#endif

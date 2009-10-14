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

/* ***** Changelog: huffcodec.cpp *****
 * 2009.09.30 - v1 r0
 * 		Intitial Release
 */

#include "huffcodec.h"

// implementation
namespace skulltag {

// HuffmanCodec Implementation

	/** Reverses the order of bits in a byte.
	 *	EG: The statement <code>reverseMap[0xAF] == 0xF5</code> is <code>true</code>. <br>
	 *	The index <code>10101111</code> stores the reverse value: <code>11110101</code>. <br>
	 *  Note: One array lookup is much faster than Eight bit manipulating loop iterations. */
	unsigned char const HuffmanCodec::reverseMap[] = {
		  0,128, 64,192, 32,160, 96,224, 16,144, 80,208, 48,176,112,240,
		  8,136, 72,200, 40,168,104,232, 24,152, 88,216, 56,184,120,248,
		  4,132, 68,196, 36,164,100,228, 20,148, 84,212, 52,180,116,244,
		 12,140, 76,204, 44,172,108,236, 28,156, 92,220, 60,188,124,252,
		  2,130, 66,194, 34,162, 98,226, 18,146, 82,210, 50,178,114,242,
		 10,138, 74,202, 42,170,106,234, 26,154, 90,218, 58,186,122,250,
		  6,134, 70,198, 38,166,102,230, 22,150, 86,214, 54,182,118,246,
		 14,142, 78,206, 46,174,110,238, 30,158, 94,222, 62,190,126,254,
		  1,129, 65,193, 33,161, 97,225, 17,145, 81,209, 49,177,113,241,
		  9,137, 73,201, 41,169,105,233, 25,153, 89,217, 57,185,121,249,
		  5,133, 69,197, 37,165,101,229, 21,149, 85,213, 53,181,117,245,
		 13,141, 77,205, 45,173,109,237, 29,157, 93,221, 61,189,125,253,
		  3,131, 67,195, 35,163, 99,227, 19,147, 83,211, 51,179,115,243,
		 11,139, 75,203, 43,171,107,235, 27,155, 91,219, 59,187,123,251,
		  7,135, 71,199, 39,167,103,231, 23,151, 87,215, 55,183,119,247,
		 15,143, 79,207, 47,175,111,239, 31,159, 95,223, 63,191,127,255
	};

	/** Creates a new HuffmanCodec
	 * @param treeData char array containing the tree data to use.
	 * @param dataLength number of chars in treeData. */
	HuffmanCodec::HuffmanCodec( unsigned char const * const treeData, int dataLength ) : Codec::Codec() {
		codeBuffer = new BitWriter();
		// init code table (256 pointer to Huffman Nodes.)
		codeTable = new HuffmanNode*[256];
		for (int i = 0; i < 256; i++) codeTable[i] = 0;
		// build tree.
		root = new HuffmanNode;
		root->bitCount = 0;
		root->code = 0;
		root->value = -1;
		buildTree( root, treeData, 0, dataLength );
		reverseBits = false;
		expandable = true;
	}

	/** Recursively builds a Huffman Tree.
	 * @param node		A branch node of the Huffman Tree.
	 * @param treeData	A char array containing the Huffman Tree's byte representation.
	 * @param index		Current array element to read the next tree node from.
	 * @return the next index to read from or -1 if an error occurs. */
	int HuffmanCodec::buildTree( HuffmanNode * node, unsigned char const * const treeData, int index, int dataLength ){
		if ( index >= dataLength ) return -1;
		// Read the branch description bit field
		int desc = treeData[index];
		index++;

		// Create the array that will hold L/R child nodes of this branch.
		node->branch = new HuffmanNode[2];

		// Read the child Nodes for this branch.
		for ( int i = 0; i < 2; i++ ){
			// Increase bit count, and update huffman code to match the node's tree position.
			node->branch[i].bitCount = node->bitCount + 1;
			node->branch[i].code = (node->code << 1) | i; // appends a 0 or 1 depending on L/R branch.
			node->branch[i].value = -1; // default value.

			// Test a bit from the branch description (least significant bit == left)
			if ( (desc & (1 << i)) == 0 ){
				// Child node is a branch; Recurse.
				if ( (index = buildTree( &(node->branch[i]), treeData, index, dataLength )) < 0 ) return -1;
				// This means the entire left sub tree will be read before the right sub tree gets read.
			} else {
				// Read leaf value and map its value/index in the nodes array.
				if ( index >= dataLength ) return -1;
				// set the nodes huffman code values.
				node->branch[i].code = (node->code << 1) | i;
				node->branch[i].bitCount = node->bitCount+1;
				node->branch[i].value = treeData[index] & 0xff;
				// NULL the child node's branch to mark it as a leaf.
				node->branch[i].branch = 0;
				// store a pointer to the leaf node into the code table at the location of its byte value.
				codeTable[ node->branch[i].value ] = &node->branch[i];
				index++;
			}
		}

		return index;
	}

	/** Decodes data read from an input buffer and stores the result in the output buffer.
	 * @return number of bytes stored in the output buffer or -1 if an error occurs while encoding. */
	int HuffmanCodec::encode(
		unsigned char const * const input,	/**< in: pointer to the first byte to encode. */
		unsigned char * const output,		/**< out: pointer to an output buffer to store data. */
		int const &inLength,				/**< in: number of bytes of input buffer to encoded. */
		int const &outLength				/**< in: maximum length of data to output. */
	) const {
		// setup the bit buffer to output. if not expandable Limit output to input length.
		if ( expandable ) codeBuffer->outputBuffer( output, outLength );
		else codeBuffer->outputBuffer( output, ((inLength + 1) < outLength) ? inLength + 1 : outLength );

		codeBuffer->put( (unsigned char)0 ); // reserve place for padding signal.

		HuffmanNode * node; // temp ptr cache;
		for ( int i = 0; i < inLength; i++ ){
			node = codeTable[ 0xff & input[i] ]; //lookup node
			// Put the huffman code into the bit buffer and bail if error occurs.
			if ( !codeBuffer->put( node->code, node->bitCount ) ) return -1;
		}
		int bytesWritten, padding;
		if ( codeBuffer->finish( bytesWritten, padding ) ){
			// write padding signal byte to begining of stream.
			output[0] = (unsigned char)padding;
		} else return -1;

		// Reverse the bit order of each byte (Old Huffman Compatibility Mode)
		if ( reverseBits ) for ( int i = 1; i < bytesWritten; i++ ){
			output[i] = reverseMap[ 0xff & output[i] ];
		}

		return bytesWritten;
	}

	/** Decodes data read from an input buffer and stores the result in the output buffer.
	 * @return number of bytes stored in the output buffer or -1 if an error occurs while decoding. */
	int HuffmanCodec::decode(
		unsigned char const * const input,	/**< in: pointer to data that needs decoding. */
		unsigned char * const output,		/**< out: pointer to output buffer to store decoded data. */
		int const &inLength,				/**< in: number of bytes of input buffer to read. */
		int const &outLength				/**< in: maximum length of data to output. */
	){
		if ( inLength < 1 ) return 0;
		int bitsAvailable = ((inLength-1) << 3) - (0x07 & input[0]);
		int rIndex = 1;		// read index of input buffer.
		int wIndex = 0;		// write index of output buffer.
		char byte = 0;		// bits of the current byte.
		int bitsLeft = 0;	// bits left in byte;

		HuffmanNode * node = root;

		// Traverse the tree, output values.
		while ( (bitsAvailable > 0) && (node != 0) ){

			// Get the next byte if we've run out.
			if ( bitsLeft <= 0 ){
				byte = input[rIndex++];
				if ( reverseBits ) byte = reverseMap[ 0xff & byte ];
				bitsLeft = 8;
			}

			// Traverse the tree according to the most significant bit.
			node = &(node->branch[ ((byte >> 7) & 0x01) ]);

			// Is the node Non NULL, and a leaf?
			if ( (node != 0) && (node->branch == 0) ){
				// Output leaf node's value and restart traversal at root node.
				output[ wIndex++ ] = (unsigned char)(node->value & 0xff);
				node = root;
			}

			byte <<= 1;			// cue up the next bit
			bitsLeft--;			// use up one bit of byte
			bitsAvailable--;	// decrement total bits left
		}

		return wIndex;
	}

	/** Deletes a Huffman Tree structure by traversing and deleting its nodes. */
	void HuffmanCodec::deleteTree( HuffmanNode * root ){
		if ( root == 0 ) return;
		if ( root->branch != 0 ){
			deleteTree( &(root->branch[0]) );
			deleteTree( &(root->branch[1]) );
			delete root->branch;
		}
	}

	/** Destructor - frees resources. */
	HuffmanCodec::~HuffmanCodec() {
		delete codeBuffer;
		delete codeTable;
		deleteTree( root );
		delete root;
	}

	/** Enables or Disables backwards bit ordering of bytes.
	 * @param backwards  "true" enables reversed bit order bytes, "false" uses standard byte bit ordering. */
	void HuffmanCodec::reversedBytes( bool backwards ){ reverseBits = backwards; }

	/** Check the state of backwards bit ordering for bytes.
	 * @return  true: bits within bytes are reversed. false: bits within bytes are normal. */
	bool HuffmanCodec::reversedBytes(){ return reverseBits; }

	/** Enable or Disable data expansion during encoding.
	 * @param expandingAllowed	"true" allows encoding to expand data. "false" causes failure upon expansion. */
	void HuffmanCodec::allowExpansion( bool expandingAllowed ){ expandable = expandingAllowed; }

	/** Check the state of data expandability.
	 * @return	 true: data expansion is allowed.  false: data is not allowed to expand. */
	bool HuffmanCodec::allowExpansion(){ return expandable; }


}; // end namespace skulltag

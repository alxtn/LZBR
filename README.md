# LZBR

LZ77 compressor utilizing byte reservation and an alternate match
encoding scheme to enchance compression ratios

## Structure

+ First, dictionary matches are found using a sliding window (like LZ77)
	+ These are categorized as: length 3, length 4 and length > 4
+ Next, the data is transformed to ensure certain byte values do not appear
+ Finally, matches are encoded, and the data is packed into it's output format

## Byte Reservation

Byte values are reserved by breaking the data into blocks, then
a set is created containing the byte values not present in the
block. A byte value is then found that, when XOR'd with the data, maps
the missing set onto the set to be reserved. In the case that a
solution is not found, a smaller block should be taken. By XOR'ing
each byte of data with the solution value, the reserved byte values are
guaranteed to not occur in that block.

## Match Chaining

The LZ77 output format consists of a stream of tokens that are either
one-byte literals, or three-byte matches (Composed of a length and
an offset). Preceeding each group of 8 tokens is a byte of flags
indicating which tokens are literals and which are dictionary matches.

The output format in LZBR leverages the reserved byte values to
eliminate the need for the byte of flags in LZ77. Three byte values
are reserved, and are used to mark matches of length 3, length 4, and
length > 4 repectively.

Additionally, since the length of the match is known for matches of
length 3 and length 4, several bits are freed up in the length field
of the match token. These bit are used to "chain" matches, specifying the
offset to the next match of the same length (if it is within
range). This eliminates the need to have an entire byte dedicated to
marking each dictionary match token.
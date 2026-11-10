print   lda qstion
		brz write
		out
		lda print
		add one
		sta print
		bra print

write	inp
		sub ETX
		brz end
		sub RBS
		brz back
		sub RCR
		brz end
		sub RIO
		brz io
		add IO

		out

		lda ans_len
		add one
		sta ans_len
		bra write
end		hlt

io		inp
		
		sub ILEFT
		brz left
		// sub RIRIGHT
		// brz right

		bra write

left	lda BS
		out
		bra write

back	lda ans_len
		brz bell
		lda BS
		out
		lda SPC
		out
		lda BS
		out

		lda ans_len
		sub one
		sta ans_len
		bra write

bell	lda BEL
		out
		bra write

CR		dat 13

ETX		dat 3
RBS		dat 5
RCR		dat 5
RIO		dat 211
IO		dat 224

ILEFT	dat 75
RIRIGHT	dat 2

BEL		dat 7
BS		dat 8
DEL		dat 127
SPC		dat 32

ans_len	dat 0
one		dat 1

qstion  dat 87
		dat 104
		dat 97
		dat 116
		dat 39
		dat 115
		dat 32
		dat 121
		dat 111
		dat 117
		dat 114
		dat 32
		dat 110
		dat 97
		dat 109
		dat 101
		dat 63
		dat 10
		dat 62
		dat 32
		dat 0
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
		brz wans1
		sub RIO
		brz io
		add IO

		out

rcurs	lda cursor
		add one
		sta cursor
		bra write

end		hlt

io		inp
		
		sub ILEFT
		brz left
		// sub RIRIGHT
		// brz right

		bra write

left	lda cursor
		brz write
		lda BS
		bra lcurs

back	lda cursor
		brz bell
		lda BS
		out
		lda SPC
		out
		bra left

lcurs	lda cursor
		sub one
		sta cursor
		bra write

bell	lda BEL
		out
		bra write

wans1	lda answer1
		brz wname
		out
		lda wans1
		add one
		sta wans1
		bra wans1

wname	lda name
		brz wans2
		out
		lda wname
		add one
		sta wname
		bra wname

wans2	lda answer2
		brz end
		out
		lda wans2
		add one
		sta wans2
		bra wans2

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

cursor	dat 0
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

answer1 dat 72
		dat 101
		dat 108
		dat 108
		dat 111
		dat 32
		dat 116
		dat 104
		dat 101
		dat 114
		dat 101
		dat 32
		dat 0

answer2 dat 44
		dat 32
		dat 104
		dat 111
		dat 119
		dat 32
		dat 97
		dat 114
		dat 101
		dat 32
		dat 121
		dat 111
		dat 117
		dat 32
		dat 100
		dat 111
		dat 105
		dat 110
		dat 103
		dat 63
		dat 0

name	dat 0
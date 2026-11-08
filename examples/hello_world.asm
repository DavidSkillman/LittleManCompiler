print	lda string
		brz end
        otc
		lda print
		add one
		sta print
		bra print
end		hlt

string  dat 72
        dat 101
        dat 108
        dat 108
        dat 111
        dat 32
        dat 87
        dat 111
        dat 114
        dat 108
        dat 100
        dat 33
        dat 0
one     dat 1
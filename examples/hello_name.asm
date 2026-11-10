write	inp
		sub ETX
		brz end
		add ETX
		sub LF
		brz end
		add LF
		out
		bra write
end		hlt

LF		dat 10
ETX		dat 3
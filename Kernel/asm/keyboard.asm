GLOBAL get_key

get_key:
	push rbp
	mov rbp,rsp
	in al, 60h
	movzx rax, al
	mov rsp,rbp
	pop rbp
	ret

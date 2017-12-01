GLOBAL _unlocked

_unlocked:
  mov rax, 0
  xchg rax, [rdi]
	ret

    nop
    nop

    CLD								// 清空 DF 标志位
    push 0x1e380a6a					// MessageBoxA 的hash
    push 0x4fd18963					// ExitProcess 的hash
    push 0x0c917432					// LoadlibraryA 的hash
    mov esi, esp					// esi 指向了 LoadlibraryA 的hash
    lea edi, [esi - 0xC]			// edi 指向了 MessageBoxA 的hash

    xor ebx, ebx
    mov bh, 0x04
    sub esp, ebx					// ebx = 0x00000400

    mov bx, 0x3233					// ebx = 0x00003233
    push ebx
    push 0x72657375					// 此时内存 75736572 33320000  
    push esp						// 保存 user32 字符串的地址
    xor edx, edx

    mov ebx, fs:[edx + 0x30]		// ebx 指向 PEB
    mov ecx, [ebx + 0x0C]			// ecx 指向 LDR
    mov ecx, [ecx + 0x1C]			// ecx 指向第一个结点，该结点首地址指向第二个结点

    mov ecx, [ecx]					// ecx 指向第二个结点
    mov ebp, [ecx + 0x08]			// ebp 指向 kernel32.dll 基址


find_lib_functions:

    lodsd							// 加载 esi 指向的内存到 eax 中， 并且 esi = esi + 4
    cmp eax, 0x1e380a6a             // 如果查找的函数为 MessageBoxA 则调用 LoadlibraryA 加载 user32 模块             
    jne find_functions
    xchg eax, ebp
    call [edi - 0x8]                // 调用 LoadlibraryA 
    xchg eax, ebp                   // ebp 始终保存模块基址                 

find_functions:
    pushad                          // 所有通用寄存器入栈
    mov eax, [ebp + 0x3C]			// 获取模块的 PE 头
    mov ecx, [ebp + eax + 0x78]		// 获取导出表指针偏移
    add ecx, ebp					// 获取导出表的实际位置
    mov ebx, [ecx + 0x20]			// 导出函数函数名的列表偏移
    add ebx, ebp					// 导出函数函数名的列表的实际位置
    xor edi, edi

next_function_loop:
    inc edi
    mov esi, [ebx + edi *4]			// 导出函数函数名相对偏移
    add esi, ebp					// 实际位置
    cdq

hash_loop:							// hash 函数
    movsx eax, byte ptr[esi]
    cmp al , ah
    jz compare_hash
    ror edx, 7
    add edx, eax
    inc esi
    jmp hash_loop

compare_hash:
    cmp edx, [esp + 0x1C]			
    jnz next_function_loop
    mov ebx, [ecx + 0x24]           // 解析 PE 
    add ebx, ebp
    mov di, [ebx + 2 * edi]
    mov ebx, [ecx + 0x1c]
    add ebx, ebp
    add ebp, [ebx + 4 * edi]

    xchg eax, ebp
    pop edi
    stosd
    push edi
    popad                           // 所有通用寄存器出栈
    cmp eax, 0x1e380a6a             // 是否找到最后一个函数
    jne find_lib_functions          // 查找下一个函数

function_call:
    xor ebx, ebx	
    xor eax, eax                    // 清零
    mov al, 0x74                    // 构造出 push 0x00000074 即 t
    push eax		
    push 0x61434B43	                // 构造出 CKCa

    mov eax, esp  
    push ebx
    push eax
    push eax
    push ebx
    call [edi - 0x04]	            // MessageBoxA

    push ebx
    call [edi - 0x08]               // ExitProcess

    nop
    nop


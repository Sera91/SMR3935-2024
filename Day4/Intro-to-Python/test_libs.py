import numpy as np
import cupy as cp
import torch
import time

bs = 8
L = 2048
dim = 64


print(torch.cuda.is_available())

tensor1 = torch.randn((bs, L, dim)).to('cuda')
tensor2 = torch.randn((L, L, dim)).to('cuda')

matrix1 = torch.randn((L, L)).to('cuda')
matrix2 = torch.randn((L, L)).to('cuda')

# warmup the GPU
for _ in range(5):
    warump_tensor = torch.matmul(tensor1, tensor1.transpose(1, 2))


#testing batch matrix multiplication with einsum
torch.cuda.synchronize()
start = time.time()
output1 = torch.einsum("bld,lrd->blr", tensor1, tensor2)
torch.cuda.synchronize()
end = time.time()
print('einsum time:', end-start)
print('einsum memory (GB):', torch.cuda.max_memory_allocated('cuda')/10**9)



print("program finished")

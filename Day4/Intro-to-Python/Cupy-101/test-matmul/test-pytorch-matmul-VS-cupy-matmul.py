import torch
import time
import cupy as cu
from torch.utils.dlpack import to_dlpack
from torch.utils.dlpack import from_dlpack


L = 2048



print(torch.cuda.is_available())


matrix1 = torch.randn((L, L)).to('cuda')
matrix2 = torch.randn((L, L)).to('cuda')

# warmup the GPU
for _ in range(5):
    warmup_tensor = torch.matmul(matrix1, matrix2)

del warmup_tensor



#testing matrix multiplication with einsum
torch.cuda.synchronize()
start = time.time()
output_torch = torch.matmul(matrix1, matrix2)
torch.cuda.synchronize()
end = time.time()
print('torch matmul time:', end-start)
print('matmul memory (GB):', torch.cuda.max_memory_allocated('cuda')/10**9)


matrix1_cupy = cu.fromDlpack(to_dlpack(matrix1))
del matrix1

matrix2_cupy = cu.fromDlpack(to_dlpack(matrix2))
del matrix2

cu.cuda.runtime.deviceSynchronize()
start = time.time()
matrix_cupy=cu.matmul(matrix1_cupy, matrix2_cupy)
end = time.time()
print('cupy matmul time:', end-start)

output_cupy = from_dlpack(matrix_cupy.toDlpack())

print('same res from matmul?', torch.allclose(output_torch, output_cupy, atol=1e-5))

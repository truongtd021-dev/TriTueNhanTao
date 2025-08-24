import numpy as np

# Sinh ngau nhien du lieu
duLieu = np.random.randint(10, 100, size=(2, 7))

print("So luong hang ban (2 x 7):")
print("Sang :", duLieu[0])
print("Chieu:", duLieu[1])

# a
tongNgay = duLieu.sum(axis=0)
ngayMax = np.argmax(tongNgay) + 1
print(f"\n(a) Ngay ban duoc nhieu nhat: Ngay {ngayMax}, tong = {tongNgay[ngayMax-1]}")

# b
i, j = np.unravel_index(np.argmax(duLieu), duLieu.shape)
buoi = "Sang" if i == 0 else "Chieu"
print(f"(b) Thoi diem ban nhieu nhat: {buoi} ngay {j+1}, so luong = {duLieu[i, j]}")

# c
tongSang = duLieu[0].sum()
tongChieu = duLieu[1].sum()

print("\n(c)", end=" ")
if tongSang > tongChieu:
    print(f"Buoi sang ban nhieu hon (tong {tongSang} so voi {tongChieu})")
elif tongSang < tongChieu:
    print(f"Buoi chieu ban nhieu hon (tong {tongChieu} so voi {tongSang})")
else:
    print(f"Hai buoi bang nhau (cung {tongSang})")

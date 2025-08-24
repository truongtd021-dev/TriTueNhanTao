# Bài 2: Tính điểm trung bình và phân loại
d1 = float(input("Nhap diem Dai so tuyen tinh: "))
d2 = float(input("Nhap diem Nhap mon lap trinh: "))
d3 = float(input("Nhap diem Tieng anh: "))

dtb = (d1 + d2 + d3) / 3
print("Diem trung binh:", round(dtb, 2))

if dtb >= 8.5:
    print("Xep loai: Gioi")
elif dtb >= 7.0:
    print("Xep loai: Kha")
elif dtb >= 5.0:
    print("Xep loai: Trung binh")
else:
    print("Xep loai: Yeu")

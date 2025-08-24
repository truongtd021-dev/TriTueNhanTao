n = int(input("nhap vao 1 so nguyen: "))
if n < 2:
    print(n, "ko la so nguyen to")
else:
    dung = true
    for i in range(2, int(n**0.5) + 1):
        if n % i == 0:
            dung = False
            break
    if dung:
        print(n, "la so nguyen to")
    else:
        print(n, "ko la so nguyen to")
def calculate_steps(distance, stride_length):
    steps = distance / stride_length
    return steps

def main():
    stride_length = float(input("歩幅（メートル）を入力: "))
    distance = float(input("歩いた距離（メートル）を入力: "))
    
    steps = calculate_steps(distance, stride_length)
    print(f"歩数: {steps} 歩")
    
    if steps > 10000:
        print("歩数が1万歩を超えています！㊗")
    else:
        print("歩数が1万歩未満です。")

if __name__ == "__main__":
    main()

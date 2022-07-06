import sys
from collections import deque
import re
input = sys.stdin.readline


file=open('input.txt','r')  # txt 파일을 읽기 모드로 파일 열기 . 

s=file.read() # txt 파일의 문자들을 읽어온다.
print('----------------------')
print('s',s)
print('----------------------')


mem=[[0]*3 for _ in range(15)]
register=[0]*10

arr=s.split()

count=0
index=-1
global idx
idx=0
for i in arr: 
    if 'R' in i or '0' in i :
        mem[index][count]=i
        count+=1
        
    elif i =='+' or i=='-' or i=='/' or i=='*':
        count=0
        index+=1
        mem[index][count]=i
        count+=1
                
    elif i.isdigit()==False:
        count=0
        index+=1
        mem[index][count]=i
        count+=1
    
  



def m(mem):# move 함수
    global idx

    num1=mem[idx][1]
    print(num1, end=' ')
    num1=re.findall("\d+",num1)
    num1=int(num1[0])
    num2=int(mem[idx][2],16)
    print(num2, end=' ')
    register[num1]=num2

    print('-> R',num1,':',num2)
    

def c(mem):
    global idx
    num1=arr[idx]
    print(num1, end=' ')
    idx+=1
    if 'R' in num1: # 레지스터의 값의 연산인지 숫자간의 연산인지를 판단하기위한 조건문 
        num1=re.findall("\d+",num1)
        num1=int(num1[0])
        num1=register[num1]
    else:
        num1=int(num1,16)
    num2=arr[idx]
    print(num2)
    if 'R' in num2:
        num2=re.findall("\d+",num2)
        num2=int(num2[0])
        num2=register[num2]
    else:
        num2=int(num2,16)

    if num1 >= num2:
        register[0]=0
    else:
        register[0]=1
        
def j(mem):
    global idx
    print(mem[idx][1])
    
    idx=int(mem[idx][1],16)-2


def b(mem):
    global idx
    num=mem[idx][0]
    num=re.findalll("\d+",num)
    if register[0]==1:
        j(mem)
    

def ch(mem):
    global idx
    num1=mem[idx][1]
    print(num1, end=' ')
   
    
    if 'R' in num1: # 레지스터의 값의 연산인지 숫자간의 연산인지를 판단하기위한 조건문 
        num1=re.findall("\d+",num1)
        num1=int(num1[0])
      
        
    else:
        num1=int(num1,16)
    num2=mem[idx][2]
    print(num2)
 
    if 'R' in num2:
        num2=re.findall("\d+",num2)
        num2=int(num2[0])
      
    else:
        num2=int(num2,16)

    tem=0
    tem=register[num1]

    register[num1]=register[num2]
    register[num2]=tem

    
def gcd(mem):
    global idx
    num1=mem[idx][1]
    print(num1, end=' ')
   
    
    if 'R' in num1: # 레지스터의 값의 연산인지 숫자간의 연산인지를 판단하기위한 조건문 
        num1=re.findall("\d+",num1)
        num1=int(num1[0])
      
    else:
        num1=int(num1,16)
    num2=mem[idx][2]
    print(num2, end=' ')
 
    if 'R' in num2:
        num2=re.findall("\d+",num2)
        num2=int(num2[0])
      
    else:
        num2=int(num2,16)

    if num1==num2:
        print("a")
    if num1 < num2:
        ch(mem)

    tnum1=register[num1]
    tnum2=register[num2]
    while tnum2 !=0:
        n=tnum1%tnum2
        tnum1=tnum2
        tnum2=n

    print('-> result of the gcd',tnum1)   



idx=0
while idx !=len(mem):
    count=0
    x=mem[idx][count]
    count+=1

    if x =='+' or x=='-' or x=='/' or x=='*':# 문자열중 사칙연산을 하는지 조건문으로 판다.
        print(x,end=' ')
        num1=mem[idx][count]
        print(num1,end=' ')
    
        if 'R' in num1: # 레지스터의 값의 연산인지 숫자간의 연산인지를 판단하기위한 조건문
            
            
            num1=re.findall("\d+",num1)
            num1=int(num1[0])
            num1=register[num1]
         
 
            
        else:
            
            num1=int(num1,16)
   
        count+=1

        num2=mem[idx][count]
        print(num2,end=' ')
        if 'R' in num2:
            num2=re.findall("\d+",num2)
            num2=int(num2[0])
            num2=register[num2]
         
        else:
            num2=int(num2,16)
        count=0
        
        
     
        if x=='+':
            register[0]=num1+num2
            
        elif x=='-':
            register[0]=num1-num2
        elif x=='/':
            register[0]=num1/num2
        elif x=='*':
            register[0]=num1*num2
        print( '-> register[0]: ', register[0])
            
        
       
    

    else:# 사칙연산 이외의 경우 ( 명령어 구현 )
        if x=='M':
            print('M ',end=' ')
            
            m(mem)
           
        #추가되는 함수 조건문
        if x=='C':
            print('C')
            c(mem)

        if x=='J':
            print('J',end=' ')
            j(mem)

        if x=='B':
            print('B', end=' ')
            b(mem)
        
        if x=='CH':
            print('CH',end=' ')
            ch(mem)

        if x=='GCD':
            print('GCD',end=' ')
            gcd(mem)

        if x=='H':
            print("complete execution")
            break
        
    
    idx+=1
    #print('\n')
    #print(mem,'!!!!!!',register)
    
    
file.close()
print('register:',register)

    

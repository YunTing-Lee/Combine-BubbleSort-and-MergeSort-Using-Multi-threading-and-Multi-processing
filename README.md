# Merge-Sort-Using-Multi-threading-and-Multi-processing

## 開發環境
- 使用的程式語言: C++
- 虛擬機器軟體 : Oracle VM VirtualBox
- 作業系統 : Ubuntu 18.04.05(64bit)

## 使用流程
1. 編譯 : g++ multi-threading and multi-processing.cpp -o out -lpthread
2. 執行 : ./out
3. 螢幕會顯示提示字元 :「請輸入檔案名稱(eg.input_1w, input_10w,......)[0]Quit :」，請使用者輸入檔名，不用加上附檔名，若輸入的檔案找不到，則會顯示「###檔名does not exist###」，請使用者再輸入一次，直到輸入正確為止進到下一步驟或是輸入0自動結束程式。
4. 螢幕會顯示提示字元 :「請問要切成幾份(正整數) : 」，請使用者輸入要將檔案內資料分成幾份，此數字必須是正整數，若不是正整數或此數字超過檔案數字比數，會顯示「error ! Please try again :」，請使用者再輸入一次，直到輸入正確為止，才會進到下一個步驟。
5. 螢幕會顯示提示字元 :「請輸入方法編號 (方法1, 方法2, 方法3, 方法4) :  」，請使用者輸入要排序的方法，此數字必須是1, 2, 3, 4其中一個，若不是，則會顯示「error ! Please try again :」，請使用者再輸入一次，直到輸入正確為止，才會進到下一個步驟。
6. 開始做使用者所指定方法的排序，排序結束會顯示「Finished Sorting !」，並顯示排序所花的時間，接著開始把排序好的結果寫入一個新的檔案內，檔名為原始輸入檔名+”_output”+方法。
7. 重複3. – 6. 步驟

## output檔案格式 : 
- output輸出檔名 : {input檔名} + "output" + {方法號碼}
- 內容為排序好的數字，最後加上CPU Time與Date Time(寫檔當下的時間)

## 方法一 : 
將N個數目字直接進行Bubble Sort

## 方法二 :
將N個數目先切成K份，在一個Process內對K份資料做Bubble Sort之後，再用同一個Process作Merge Sort

## 方法三 : 
將N個數目字切成K份，並由K個Processes分別進行Bubble Sort之後，再用Process(es)作Merge Sort

## 方法四 : 
將N個數目字先成K份，並由K個Threads對分別進行Bubble Sort之後，再用Thread(s)作Merge Sort

---
## 分析結果與原因 : 
### **[不同N值 vs. 執行時間] :**  
![image](https://github.com/YunTing-Lee/Merge-Sort-Using-Multi-threading-and-Multi-processing/blob/main/Picture/Time%20spent%20in%20different%20methods%20for%20different%20numbers%20of%20data(K%3D1000).png)
![image](https://github.com/YunTing-Lee/Merge-Sort-Using-Multi-threading-and-Multi-processing/blob/main/Picture/line%20chart%20_time%20spent%20in%20different%20methods%20for%20different%20numbers%20of%20data(K%3D1000).png)

**探討造成執行速度差別的原因，以及可能的解決方法 :**  
在大資料(100萬筆)的測試下，若K值設為1000，執行效率為 方法3 > 方法2 > 方法4 > 方法1，結果顯示使用方法一的直接全部bubble sort是最慢的，這是可預期的。而在另外3個方法中，使用方法3(Multi-process)會是最快的，而方法2(一個process)與方法3(Multi-thread)是差不多的。


**Multi-process比Multi-thread快的原因可能有以下2個 :**
1. CPU scheduler依process為單位分配time slice  
	因為我們自己寫的thread會是user thread，而user thread會依附在process內，所以CPU scheduler還是會以process為單位進行scheduling, context switch 以及分配time slice。則因Multi-Thread本質上還是在一個process裡做資源分配以及優化，沒有利用到Multi-Processing，所以對於速度提升影響有限。因此雖然Multi-Process會比Multi-thread更消耗資源( 因每個process都需有自己的address space )，但因為使用Multi-Process可以會獲得更多的 CPU執行時間以及更好的利用CPU的效能，所以可以更明顯的減少排序的程式執行時間。

2. 在排序時CPU處於忙碌狀態，不會有I/O或事件發生需等待  
	因為我們要排序前會把所需要的數據資料都載入memory裡，因此在排序時是牽涉到CPU與memory間的資料交換，由於兩者間的 I/O 速度差異不大，CPU不會使用很多時間等待記憶體的資料複製工作，也因為在排序時不會需要I/O，所以CPU是隨時處於忙碌的狀態。當CPU忙碌時，使用Multi-thread反而會額外增加了資源同步的協調動作，導致效能降低，相反的Multi-process不需要多做資源同步的協調動作，因此可讓CPU專心忙碌於排序工作，使效能提升更多。  
	而一般在討論Multi-Thread因為花費context switch的代價比Multi-Process相對輕很多，所以效能會較高的說法，有個先決條件是CPU在計算過程中經常處於閒置狀態，而這樣Multi-Thread就可以降低CPU閒置的時間，做程式的其他部分的運算，所以才會提高計算效能。而由於這次的程式不符合此條件，導致Multi-Thread的優勢無法展現出來，所以Multi-Thread才會比Multi-Process慢。  

**Multi-thread與 1個process執行速度差不多的原因 :**  
除了上述講到Multi-Thread無法發揮優勢的原因之外，推測可能也與K值的大小有關。在此實驗中，不管資料筆數是多少皆將K值設為1000，而在下一個實驗中，變動了K值做比較就可以發現當K值增加後，Multi-thread的執行速度就會比只有一個process快上許多，原因可能是當K不夠大時也無法發揮Multi-thread並行執行的優勢，所以與只有一個process的執行速度差不多。

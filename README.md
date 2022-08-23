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


## 分析結果與原因 : 
[不同N值 vs. 執行時間] :
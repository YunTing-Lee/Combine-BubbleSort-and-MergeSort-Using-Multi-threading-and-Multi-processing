# include <iostream>
# include <fstream>
# include <vector>
# include <string>
# include <cstdlib>
# include <cstdio>
# include<cstring>
# include <time.h>
# include <cctype>
# include <algorithm>
# include <ctime>
# include <math.h>
# include <unistd.h>
# include <thread>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/ipc.h>
# include <sys/shm.h>
# include <sys/time.h>

using namespace std;


void ReadUserInput( string & inputFullName, string & fileId, vector<int> & unsortedNum, int & command, int & peices, bool & isQuit  ) ;
int ReadCommand() ;  // 讀Command，若是0就直接結束
void Enter() ;
void ReadInputFileName( string & fileId,string & inputFullName ) ; // 讀input檔編號
bool OpenInputFile( string & fileId,string & inputFullName,fstream & inputFile ) ;  // 開啟input檔
void ReadInputData( fstream & file, vector<int> & unsortedNum ) ;//將input檔裡的資料讀進vector<School> sch裡
string ReadString(fstream & file); // 從file裡讀一個string(以'\t'做分割)
void Swap( vector<int> & num, int a, int b ) ; // 將vector裡第a個元素與第b個元素做交換
int StringToInt( string str ) ;
bool IsDigit( char ch ) ;
bool IsAllDigit( string str ) ;
void ReadANum ( int & userNum ) ;
void BubbleSort( vector<int> & sortedNum ) ;
void WriteSortedData( vector<int> sortedNum, string fileId, int command, double sortedCPUTime ) ;
void CutDataToKPieces( vector<int> sortedNum, int pieces, vector< vector<int> > & kPiecesData ) ;
void merge2PiecesData( vector< vector<int> > & KPiecesData, int mergeIndex1, int mergeIndex2 ) ;


class MultiThread {
public :

  int part ;
  vector<int> sortedNum ;
  int howManyPieces = 0 ;
  int numOfDataInAPiece ;
  int numOfcurrentPiece ;

  MultiThread( vector<int> inputNum, int pieces ) {
    sortedNum = inputNum ;
    howManyPieces = pieces ;
    numOfcurrentPiece = pieces ;
    part = 0 ;
    numOfDataInAPiece = floor( sortedNum.size() / howManyPieces ) ;
  } // constructor


  void Swap( int a, int b ) {  // 將sch中第a個元素與第b個元素做交換
    int temp = sortedNum[a];
    sortedNum[a] = sortedNum[b] ;
    sortedNum[b] = temp ;
  } // Swap()


  void BubbleSort() {

    int threadPart = part++ ;  // 要做第幾份的bubblesort
    int low, high ; // 要做buublesort的index範圍
    if ( threadPart == howManyPieces-1 ) { // 最後一份
      low = threadPart * numOfDataInAPiece ;
      high = sortedNum.size() - 1 ;
    }
    else {
      low = threadPart * numOfDataInAPiece ;
      high = ( threadPart + 1 ) * numOfDataInAPiece - 1 ;
    } // else

    bool hasSorted = false ; // 是否排序好了
    for ( int i = low ; i < high && ! hasSorted ; i++ ) {
      hasSorted = true ;
      for( int j = low ; j < high - i + low  ; j++ ) { // 將相鄰的兩元素做比較
        if ( sortedNum[j] > sortedNum[j+1] ) { // 若前面的元素小於後面的元素
          Swap( j,j+1 ) ;   // 做交換，讓比較小的元素往後擠
          hasSorted = false ;  // 還有做交換的動作代表還沒排序好
        } // if
      } // end inner for
    } // end outer for

  } // BubbleSort()

  void KThreadBubble() {

    thread threads[howManyPieces] ;
    for ( int i = 0 ; i < howManyPieces ; i++ )
      threads[i] = thread( thread(&MultiThread::BubbleSort, this) ) ;
    for ( int i = 0 ; i < howManyPieces ; i++ )
      threads[i].join() ;

  } // KThreadBubble()

  void MergePart() {

    vector<int> temp ; // 暫存排序好的資料

    int first = part * numOfDataInAPiece, mid = (++part) * numOfDataInAPiece -1, last = (++part) * numOfDataInAPiece -1 ;
    int leftIndex = first, leftLast = mid ;   // 左半邊從位置first~mid
    int rightIndex, rightLast ; // 右半邊從位置mid+1~last

    // 設定右半部的範圍
    if ( part  == numOfcurrentPiece ) { // 最後一份
      rightIndex = mid + 1 ;
      rightLast = sortedNum.size() -1 ;
    } // if
    else if ( mid + 1 > sortedNum.size() -1 )
      rightIndex, rightLast, last = sortedNum.size() -1 ;
    else if ( last > sortedNum.size() -1 ) {
      rightIndex = mid+1 ;
      rightLast, last = sortedNum.size() -1 ;
    } // else if
    else {
      rightIndex = mid + 1 ;
      rightLast = last ; // 右半邊從位置mid+1~last
    } // else


    //比較左半邊與右半邊最前面的元素，若有一邊已經都已經比完了，就結束
    for ( leftIndex , rightIndex ; ( leftIndex <= leftLast ) && ( rightIndex <= rightLast) ;  ) {
      int leftNum = sortedNum[leftIndex] ;
      int rightNum = sortedNum[rightIndex] ;

      if ( leftNum <= rightNum ){  // 若左半邊大於等於右半邊
       temp.push_back( leftNum ) ; // 將左半邊最前面的元素放進temp裡
       leftIndex++ ;  // 將左半邊的index往後一格繼續與右半邊比較
      } // if
      else {  // 右半邊最前面的元素大於左半邊最前面的元素
        temp.push_back(rightNum) ;  // 將右左半邊最前面的元素放進temp裡
        rightIndex++ ; // 將右半邊的index往後一格繼續與左半邊比較
      } // else
    } // for

    for( leftIndex ; leftIndex <= leftLast ; leftIndex++ ) {  // 將左半邊還沒放進temp裡的，依序放進去
      temp.push_back( sortedNum[leftIndex] ) ;
    }

    for( rightIndex ; rightIndex <= rightLast ; rightIndex++ ) { // 將右半邊還沒放進temp裡的，依序放進去
      temp.push_back( sortedNum[rightIndex] ) ;
    }

    for ( int index = first, i = 0  ; i < temp.size() ; index++, i++ ) { // 將剛剛排序好存進temp裡的資料依序放回sortedNum中
      sortedNum[index] = temp[i] ;
    }
    //temp.swap(vector<int> ()) ;
  } // MergePart

  void KThreadMerge() { // 做合併

    while( numOfcurrentPiece != 1 ) {

      part = 0 ;
      int threadsNum = floor ( numOfcurrentPiece / 2 ) ; // 這次要有幾個thread分別去做合併

      thread threads[threadsNum] ;
      for ( int i = 0 ; i < threadsNum ; i++ )
        threads[i] = thread( thread( thread(&MultiThread::MergePart, this) ) ) ;

      for ( int i = 0 ; i < threadsNum ; i++ )
        threads[i].join() ;

      numOfcurrentPiece = ceil ( float(numOfcurrentPiece) / float(2) ) ; // 現在總共還有幾份
      numOfDataInAPiece = numOfDataInAPiece * 2 ;  // 一個peice內會有幾筆資料，每做合併一次就*2

    } // while
  } // KThreadMerge()
}; // MultiThread


class MuliProcess{

public :

  int part ;
  vector<int> sortedNum ;
  int howManyPieces = 0 ;
  int numOfDataInAPiece ;
  int numOfcurrentPiece ;
  int shmid ;
  key_t key = IPC_PRIVATE ;
  size_t sharedMemorySize ;
  int *shm_array; // process間共用陣列

  MuliProcess( vector<int> inputNum, int pieces ) {
    sortedNum = inputNum ;
    howManyPieces = pieces ;
    numOfcurrentPiece = pieces ;
    part = 0 ;
    numOfDataInAPiece = floor( sortedNum.size() / howManyPieces ) ;
    sharedMemorySize = sizeof(int)* inputNum.size() ;  // 需要的共享記憶體空間
  } // constructor


  void Swap( int a, int b ) {  // 將sch中第a個元素與第b個元素做交換
    int temp = shm_array[a];
    shm_array[a] = shm_array[b] ;
    shm_array[b] = temp ;
  } // Swap()

  void BubbleSort( int processPart ) { // processPart : 現在是哪一份資料要做bubblesort
    int low, high ;
    if ( processPart == howManyPieces-1 ) { // 最後一份
      low = processPart * numOfDataInAPiece ;
      high = sortedNum.size() - 1 ;
    }
    else {
      low = processPart * numOfDataInAPiece ;
      high = ( processPart + 1 ) * numOfDataInAPiece - 1 ;
    } // else

    bool hasSorted = false ; // 是否排序好了
    for ( int i = low ; i < high && ! hasSorted ; i++ ) {
      hasSorted = true ;
      for( int j = low ; j < high - i + low  ; j++ ) { // 將相鄰的兩元素做比較
        if ( shm_array[j] > shm_array[j+1] ) { // 若前面的元素小於後面的元素
          Swap( j,j+1 ) ;   // 做交換，讓比較小的元素往後擠
          hasSorted = false ;  // 還有做交換的動作代表還沒排序好
        } // if
      } // end inner for
    } // end outer for

  } // BubbleSort()

  void KProcessBubble() {

    shmid = shmget( key, sharedMemorySize, IPC_CREAT | 0666 ); // 建立process間的共用記憶體
    if (shmid == -1) {
      perror("shmget failed!!");
      exit(1); // // 表示異常退出
    } // if

    // 返回一個指向共享記憶體第一個byte的pointer
    shm_array = (int *)shmat(shmid, NULL, 0) ; // 將給定的共享記憶體段附加到呼叫程序的記憶體空間。
    if ( shm_array == (int *)-1) {
      perror("shmat failed!!");
      exit(1);  // 表示異常退出
    } // if

    for( int i = 0 ; i < sortedNum.size() ; i++ ) // 將資料放進共用空間
      shm_array[i] = sortedNum[i] ;

    pid_t pid ;

    for ( int i = 0 ; i < howManyPieces ; i++, part++ ) {
      pid = fork() ; // create children process
      if ( pid < 0 )  { // child proc not created
        perror( "Children Process not created\n");
        exit(-1); // 異常退出
      } // if
      else if ( pid == 0 ) { //
        BubbleSort( part ) ; // 對哪一個part做bubblesort
        exit(0); // 正常退出
      } // else
      else {
        int status;
        waitpid(pid, &status, 0) ; // 等待children process做完
      } // else
    } // for
  } // KProcessBubble


  void MergePart( int processPart ) {
    vector<int> temp ;

    int first = processPart * numOfDataInAPiece, mid = (++processPart) * numOfDataInAPiece -1, last = (++processPart) * numOfDataInAPiece -1 ;
    int leftIndex = first, leftLast = mid ;   // 左半邊從位置first~mid
    int rightIndex, rightLast ; // 右半邊從位置mid+1~last

    if ( processPart  == numOfcurrentPiece ) { // 最後一份
      rightIndex = mid + 1  ;
      rightLast = sortedNum.size() -1 ;
    } // if
    else if ( mid + 1 > sortedNum.size() -1 )
      rightIndex, rightLast, last = sortedNum.size() -1 ;
    else if ( last > sortedNum.size() -1 ) {
      rightIndex = mid+1 ;
      rightLast, last = sortedNum.size() -1 ;
    } // else if
    else {
      rightIndex = mid + 1 ;
      rightLast = last ; // 右半邊從位置mid+1~last
    } // else



    for ( leftIndex , rightIndex ; ( leftIndex <= leftLast ) && ( rightIndex <= rightLast) ;  ) { //比較左半邊與右半邊最前面的元素，若有一邊已經都已經比完了，就結束
      int leftNum = shm_array[leftIndex] ;
      int rightNum = shm_array[rightIndex] ;

      if ( leftNum <= rightNum ){  // 若左半邊大於等於右半邊
       temp.push_back( leftNum ) ; // 將左半邊最前面的元素放進temp裡
       leftIndex++ ;  // 將左半邊的index往後一格繼續與右半邊比較
      } // if
      else {  // 右半邊最前面的元素大於左半邊最前面的元素
        temp.push_back(rightNum) ;  // 將右左半邊最前面的元素放進temp裡
        rightIndex++ ; // 將右半邊的index往後一格繼續與左半邊比較
      } // else
    } // for

    for( leftIndex ; leftIndex <= leftLast ; leftIndex++ )  // 將左半邊還沒放進temp裡的，依序放進去
      temp.push_back( shm_array[leftIndex] ) ;

    for( rightIndex ; rightIndex <= rightLast ; rightIndex++ ) // 將右半邊還沒放進temp裡的，依序放進去
      temp.push_back( shm_array[rightIndex] ) ;

    for ( int index = first, i = 0  ; i < temp.size() ; index++, i++ ) { // 將剛剛排序好存進temp裡的資料依序放回shm_array中
      shm_array[index] = temp[i] ;
    } // for

    //temp.swap(vector<int> ()) ;
  } // MergePart

  void KProcessMerge() {

    pid_t pid ;

    for( numOfcurrentPiece ; numOfcurrentPiece != 1 ;
         numOfcurrentPiece = ceil ( float(numOfcurrentPiece) / float(2) ), numOfDataInAPiece = numOfDataInAPiece * 2 ) {

      int processNum = floor ( numOfcurrentPiece / 2 ) ;
      part = 0 ;
      for ( int i = 0 ; i < processNum ; i++, part+=2 ) {

        pid = fork() ;
        if ( pid < 0 ) { // Children Proccess not created
          perror( "Children Proccess not created\n");
          exit(-1); // 異常退出
         } // if
         else if ( pid == 0 ) {
           MergePart( part ) ;
           exit(0); // 正常退出
         } // else
         else {
           int status;
           waitpid(pid, &status, 0); // Wait for child processes to finish
         } // else
      } // for
    } // for


    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
      perror("shmctl failed!!");
      exit(1);
    } // if

  } // KThreadMerge()


}; // class MultiProcess

int main() {
    string inputFullName, fileId ;
    int command = 0, pieces = 0 ;
    bool isQuit = false ;
    fstream inputFile ;
    vector<int> sortedNum ;
    double sortedCPUTime ;
    ReadUserInput( inputFullName, fileId, sortedNum, command, pieces, isQuit ) ;

    while( ! isQuit ) {

      clock_t start,terminate ;  // 計算時間用
      start = clock();


      if ( command == 1 ) {

        BubbleSort( sortedNum ) ;
        terminate = clock();
        sortedCPUTime = terminate - start ;
        cout << "Finished Sorting ! " << endl << "Sorting CPU Time : " << sortedCPUTime / CLOCKS_PER_SEC << " s" << endl ;
        WriteSortedData( sortedNum, fileId, command, sortedCPUTime ) ;
      } // if
      else if ( command == 2 ) {
        vector< vector<int> > kPiecesData ;
        CutDataToKPieces( sortedNum, pieces, kPiecesData ) ;
        for ( int i = 0 ; i < kPiecesData.size() ; i++ ) {
          BubbleSort( kPiecesData[i] ) ;
        } // for

        while( kPiecesData.size() > 1 ) {
          for( int i = 0 ; i < kPiecesData.size() -1 ; i ++ )
            merge2PiecesData( kPiecesData, i, i+1 ) ;
        } // while
        terminate = clock();
        sortedCPUTime = terminate - start ;
        cout << "Finished Sorting ! " << endl << "Sorting CPU Time : " << sortedCPUTime / CLOCKS_PER_SEC << " s" << endl ;
        WriteSortedData( kPiecesData[0], fileId, command, sortedCPUTime ) ;

      } // else if
      else if ( command == 3 ) {

        MuliProcess mulProcess( sortedNum, pieces ) ; ;
        mulProcess.KProcessBubble() ;
        mulProcess.KProcessMerge() ;
        terminate = clock();
        sortedCPUTime = terminate - start ;
        cout << "Finished Sorting ! " << endl << "Sorting CPU Time : " << sortedCPUTime / CLOCKS_PER_SEC << " s" << endl ;
        vector<int> result( mulProcess.shm_array, mulProcess.shm_array+sortedNum.size() ) ;
        WriteSortedData( result, fileId, command, sortedCPUTime ) ;

      } // else if
      else if ( command == 4 ) {

        MultiThread mulThread( sortedNum, pieces ) ;
        mulThread.KThreadBubble() ;
        mulThread.KThreadMerge() ;
        terminate = clock();
        sortedCPUTime = terminate - start ;
        cout << "Finished Sorting ! " << endl << "Sorting CPU Time : " << sortedCPUTime / CLOCKS_PER_SEC << " s" << endl  ;
        WriteSortedData( mulThread.sortedNum, fileId, command, sortedCPUTime ) ;
      } // else if

      sortedNum.clear() ;
      ReadUserInput( inputFullName, fileId, sortedNum, command, pieces, isQuit ) ;
    } // while

    cout << "Program end..." << endl ;
    return 0;
} // main()


void CutDataToKPieces( vector<int> sortedNum, int pieces, vector< vector<int> > & kPiecesData ) {

  int dataIndex = 0;
  int numOfAPieceData = floor ( sortedNum.size() / pieces ) ;  // 前k-1份會有幾個data

  for ( int i = 1 ; i <= pieces ; i++ ) {

    vector<int> newPieceData ;
    if ( i == pieces ) { // 最後一份 第k份
      while ( dataIndex < sortedNum.size() ) {
        newPieceData.push_back( sortedNum[dataIndex] ) ;
        dataIndex++ ;
      } // while
    } // if
    else { // 1 ~ (k-1) 份
      for ( int j = 0 ; j < numOfAPieceData ; j++ ) {
        newPieceData.push_back( sortedNum[ dataIndex ] ) ;
        dataIndex++ ;
      } // for
    } // else
    kPiecesData.push_back( newPieceData ) ;
    //newPieceData.swap(vector<int> ()) ;
  } // for
} // CutDataToKPieces()


void ReadUserInput( string & inputFullName, string & fileId, vector<int> & unsortedNum , int & command, int & pieces, bool & isQuit ) {
  fstream inputFile ;
  ReadInputFileName( fileId,inputFullName ) ;   // 讀檔案編號
  if ( ! OpenInputFile( fileId, inputFullName, inputFile ) )  // 開啟檔案，若檔案編號是0則直接結束重新讀command
    isQuit = true ;
  else {
    cout << "請問要切成幾份(正整數) :" ;
    ReadInputData( inputFile, unsortedNum ) ;
    inputFile.close() ;
    ReadANum( pieces ) ;
    while ( pieces <= 0 || pieces > unsortedNum.size() ) {
      cout << "Error !" << endl ;
      cout << "Please try again : " ;
      ReadANum( pieces ) ;
    } // while

    cout << "請輸入方法編號 (方法1, 方法2, 方法3, 方法4) : " ;
    ReadANum( command ) ;
    while ( command <= 0 || command > 4 ) {
      cout << "Error !" << endl ;
      cout << "Please try again :  " ;
      ReadANum( command ) ;
    } // while

    //ReadInputData( inputFile, unsortedNum ) ;
    //inputFile.close() ;
    isQuit = false ;
  } // else
} // ReadUserInput

void ReadANum( int & userNum ) {
  string numStr ;
  cin >> numStr ;
  char ch ;
  cin.get(ch);
  if ( ch != '\n')
    Enter() ;
  numStr.erase( remove_if( numStr.begin(), numStr.end(), ::isspace ), numStr.end() );

  while ( ! IsAllDigit( numStr ) ) {
    cout << "Error !" << endl ;
    cout << "Please try again : " ;
    cin >> numStr ;
    cin.get(ch);
    if ( ch != '\n')
      Enter() ;
    numStr.erase( remove_if( numStr.begin(), numStr.end(), ::isspace ), numStr.end() );
  } // while

  userNum = stol( numStr ) ;

} // ReadANum()


void Enter(){        // 讀到'\n'為止
  char enter ;
  cin.get(enter) ;
  while( enter != '\n' )
    cin.get(enter) ;
} // Enter()

void ReadInputFileName( string & fileId,string & inputFullName ){
  cout << "請輸入檔案名稱(eg.input_1w, input_10w,......)[0]Quit : " ;
  cin >> fileId ;
  char ch ;
  cin.get(ch);
  if ( ch != '\n')
    Enter() ;
  inputFullName = fileId + ".txt" ;
} // ReadInputFileName

bool OpenInputFile( string & fileId, string & inputFullName, fstream & inputFile ){

  if ( fileId.compare("0") == 0 )  // 如果編號是0，直接結束，重新讀command
   return false ;
  else {
    inputFile.open( inputFullName.c_str(), ios::in ) ;
    while ( ! inputFile ) {    // 做到檔案成功開啟
      cout << "### " << inputFullName << " does not exist ### " << endl ;
      fileId = "" ;
      ReadInputFileName( fileId,inputFullName ) ;
      if ( fileId.compare("0") == 0 )   // 如果編號是0，直接結束，重新讀command
        return false ;
      inputFile.open( inputFullName.c_str(), ios::in) ;
    } // while
  } // else
  return true ;
} // OpenFile()

void ReadInputData( fstream & file, vector<int> & unsortedNum ){

  int num = 0 ;
  while( file >> num ) {
    unsortedNum.push_back( num ) ;
  } // while
} // ReadInputData()


bool IsAllDigit( string str ) { // 是不是數字
  for( int i = 0 ; i < str.length() ; i++ ) {
    if ( str[i] < '0' || str[i] > '9')
      return false ;
  } // for
  return true ;
} // IsAllDigit()


bool IsDigit( char ch ) { // 是不是數字
  if ( ch >= '0' && ch <= '9')
    return true ;
  else
    return false ;
} // IsDigit()


void Swap( vector<int> & sortedNum, int a, int b ){  // 將sch中第a個元素與第b個元素做交換
  int temp = sortedNum[a];
  sortedNum[a] = sortedNum[b] ;
  sortedNum[b] = temp ;
} // Swap()

void BubbleSort( vector<int> & sortedNum ){ // 每次都將在未排序區域中相鄰的兩元素做比較，若前面的小於後面的，則將這兩個位置做交換，就可以把最小的元素擠到最後面

  bool hasSorted = false ; // 是否排序好了
  for ( int i = 0 ; i < sortedNum.size() - 1  && ! hasSorted ; i++ ) {
    hasSorted = true ;
    for( int j = 0 ; j < sortedNum.size() - i -1 ; j++ ) { // 將相鄰的兩元素做比較
      if ( sortedNum[j] > sortedNum[j+1] ) { // 若前面的元素小於後面的元素
         Swap( sortedNum,j,j+1) ;   // 做交換，讓比較小的元素往後擠
         hasSorted = false ;  // 還有做交換的動作代表還沒排序好
      } // if
    } // end inner for
  } // end outer for

} // bubbleSort()

void merge2PiecesData( vector< vector<int> > & kPiecesData, int mergeIndex1, int mergeIndex2 ){ // 合併左半部、右半部並排序好

  vector<int> mergeResult ; // 先將排序好的資料暫存在tempSch裡
  int leftIndex = 0, leftLast = kPiecesData[mergeIndex1].size() -1 ;  // 左半邊從位置first~mid
  int rightIndex = 0, rightLast = kPiecesData[mergeIndex2].size() -1 ; // 右半邊從位置mid+1~last

  for ( leftIndex = 0, rightIndex = 0  ; ( leftIndex <= leftLast ) && ( rightIndex <= rightLast) ;  ) { //比較左半邊與右半邊最前面的元素，若有一邊已經都已經比完了，就結束
    int leftNum = kPiecesData[mergeIndex1][leftIndex] ;
    int rightNum = kPiecesData[mergeIndex2][rightIndex] ;

    if ( leftNum <= rightNum ){  // 若左半邊大於等於右半邊
     mergeResult.push_back( leftNum ) ; // 將左半邊最前面的元素放進tempSch裡
     leftIndex++ ;  // 將左半邊的index往後一格繼續與右半邊比較
    } // if
    else {  // 右半邊最前面的元素大於左半邊最前面的元素
      mergeResult.push_back(rightNum) ;  // 將右左半邊最前面的元素放進tempSch裡
      rightIndex++ ; // 將右半邊的index往後一格繼續與左半邊比較
    } // else
  } // for

  for( leftIndex ; leftIndex <= leftLast ; leftIndex++ )  // 將左半邊還沒放進tempSch裡的，依序放進去
    mergeResult.push_back( kPiecesData[mergeIndex1][leftIndex] ) ;

  for( rightIndex ; rightIndex <= rightLast ; rightIndex++ ) // 將右半邊還沒放進tempSch裡的，依序放進去
    mergeResult.push_back( kPiecesData[mergeIndex2][rightIndex] ) ;

  kPiecesData.erase( kPiecesData.begin()+ mergeIndex1 , kPiecesData.begin() + mergeIndex2 + 1  ) ;
  kPiecesData.insert( kPiecesData.begin()+mergeIndex1, mergeResult ) ;
  //mergeResult.swap(vector<int> ()) ;
} // mergeData()

void WriteSortedData( vector<int> sortedNum, string fileId, int command, double sortedCPUTime ){

  fstream outputFile ;
  string outputFileName = fileId + "_output" + to_string( command ) + ".txt" ;
  outputFile.open( outputFileName.c_str(), ios::out ) ;
  outputFile <<  "Sort : " << endl ;

  clock_t start,terminate ;  // 計算時間用
  start = clock();
  for( int i = 0 ; i < sortedNum.size() ; i++ ) {
    outputFile << sortedNum[i] << endl ;
  } // for

  outputFile << "CPU Time : " << sortedCPUTime / CLOCKS_PER_SEC << " s" << endl ;

  char tt[100];
  time_t now = time(nullptr);
  auto tm_info = localtime(&now);

  strftime(tt, 100, "%Y-%m-%d %H:%M:%S", tm_info);
  outputFile << "Output Time : " << tt << endl ;
  outputFile.close() ;
  terminate = clock();
  sortedCPUTime = terminate - start ;
  //cout << "Finished Writing Into File! " << endl << "Writing CPU Time : " << sortedCPUTime / CLOCKS_PER_SEC << " s" << endl ;


} // WriteSortedData


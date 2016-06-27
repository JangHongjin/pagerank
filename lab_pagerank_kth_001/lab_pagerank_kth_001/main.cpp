#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#define MAXVIEW 10 //정보등을 볼때 표시할 최대갯수
#define TESTMODE 1//서버에선 0(집 데탑에선 1)
#define RANDOMJUMP 2//0.2
#define BASELINE 10 //500//500등 내 기준 출력

//#define WITCHWEIGHT 20

//#define YEARGAPSIZE 3 //이 단계에선 year gap 안씀
//#define YEARGAPNUM 3
#define WEIGHTSIZE 2
#define RESULTNUM 4//weight 탑 순위 갯수
#define TEMPN ((10-RANDOMJUMP)/WEIGHTSIZE)
#define TOTALWEIGHTNUM (TEMPN+1)*(TEMPN+2)*(TEMPN+3) / 6
//randomjump 2, weightsize가 1일때 165
//randomjump 2 weughtsuze가 2일땐 35



using namespace std;
//초기값들 !!info.txt에서 load
int filenum;
int totalpub;
int totalauth;
int totalvenue;
int totalkey;
int maxpubindexrange;
int maxauthindexrange;
int maxvenueindexrange;
int maxkeyindexrange;
int testpos;//모든 weight값이 0이아닌 pos 중 하나( 처음에 도달가능한지 확인할때 사용됨)



			//데이터들
class pubclass
{
public:
	string name;
	int hash = -1;
	int year = -1;
	string overview;
};
class authclass
{
public:
	string name;
	int hash = -1;
};
class venueclass
{
public:
	string name;
	int hash = -1;

};
class keyclass
{
public:
	string name;
	int hash = -1;

};


pubclass* publist;
authclass* authlist;
venueclass* venuelist;
keyclass* keylist;


////////////해쉬 테이블
int jshash(string name);
vector<vector<int>> hashpub;//hashpub[namehash]=pub index list
vector<vector<int>> hashauth;
vector<vector<int>> hashvenue;
vector<vector<int>> hashkey;



int** citelist;
int** aplist;
int** vplist;
int** kplist;
int** citeinvlist;
int** palist;
int** pvlist;
int** pklist;

int* citenum;
int* apnum;
int* vpnum;
int* kpnum;
int* citeinvnum;
int* panum;
int* pvnum;
int* pknum;





//편집내용 저장용 변수들




//load용 함수 및 변수들
void init();
int loadtotal();
//일반작업
void printstatus();
//pagerank 관련
class weightclass
{
public:
	int cite = (10 - RANDOMJUMP) / 4;
	int citeinv = (10 - RANDOMJUMP);
	int pa = (10 - RANDOMJUMP) / 4;
	int ap = (10 - RANDOMJUMP);
	int pv = (10 - RANDOMJUMP) / 4;
	int vp = (10 - RANDOMJUMP);
	int pk = (10 - RANDOMJUMP) / 4;
	int kp = (10 - RANDOMJUMP);
	int weightpos = TOTALWEIGHTNUM;

	int a = (10 - RANDOMJUMP); // 추가변수 for test
	int k = (10 - RANDOMJUMP);
	int v = (10 - RANDOMJUMP);
}totalweight[TOTALWEIGHTNUM];//10분의1 해서 씁니다
void pubpagerank(int currentindex);
ifstream studytarget;
ofstream studyresult;

///////////////////////////////////시작////////////////////////
int main()
{
	int inputstartnum = 0;
	int inputendnum = 0;
	int inputindex = 0;
	int inputnum = 0;
	std::ios::sync_with_stdio(false);

	cout << "load start" << endl;
	init();
	cout << "init finished " << endl;
	cout << "load total start" << endl;
	cout << "loaded:" << loadtotal() << endl;
	cout << "load finish" << endl;
	printstatus();
	studytarget.open("!!input.txt");
	studytarget >> inputstartnum;
	studytarget >> inputendnum;
	inputnum = inputendnum - inputstartnum + 1;
	for (int i = 0; i < inputstartnum; i++)
	{
		studytarget >> inputindex;
	}
	for (int i = 0; i < inputnum; i++)
	{
		studytarget >> inputindex;
		cout << inputindex << "-start" << endl;
		pubpagerank(inputindex);
		cout << inputindex << "-finished" << endl << endl;
	}
	studytarget.close();

	cout << "finished all" << endl;
	printstatus();

	getchar();
	getchar();
	getchar();
	getchar();
}
void init()
{
	string currentline;
	ifstream loadfile;
	if (TESTMODE)
	{
		loadfile.open("[test]!!info.txt");
	}
	else
	{
		loadfile.open("!!info.txt");
	}
	getline(loadfile, currentline);
	getline(loadfile, currentline);
	filenum = atoi(currentline.c_str());
	getline(loadfile, currentline);
	totalpub = atoi(currentline.c_str());
	getline(loadfile, currentline);
	totalauth = atoi(currentline.c_str());
	getline(loadfile, currentline);
	totalvenue = atoi(currentline.c_str());
	getline(loadfile, currentline);
	totalkey = atoi(currentline.c_str());


	getline(loadfile, currentline);
	maxpubindexrange = atoi(currentline.c_str()) + 1;

	getline(loadfile, currentline);
	maxauthindexrange = atoi(currentline.c_str()) + 1;

	getline(loadfile, currentline);
	maxvenueindexrange = atoi(currentline.c_str()) + 1;

	getline(loadfile, currentline);
	maxkeyindexrange = atoi(currentline.c_str()) + 1;

	loadfile.close();





#pragma omp parallel sections
	{
#pragma omp section
	{publist = new pubclass[maxpubindexrange]; }
#pragma omp section
	{authlist = new authclass[maxauthindexrange]; }
#pragma omp section
	{venuelist = new venueclass[maxvenueindexrange]; }
#pragma omp section
	{keylist = new keyclass[maxkeyindexrange]; }

#pragma omp section
	{hashpub.resize(filenum); }
#pragma omp section
	{hashauth.resize(filenum); }
#pragma omp section
	{hashvenue.resize(filenum); }
#pragma omp section
	{hashkey.resize(filenum); }
#pragma omp section
	{citelist = new int*[maxpubindexrange]; }
#pragma omp section
	{aplist = new int*[maxauthindexrange]; }
#pragma omp section
	{vplist = new int*[maxvenueindexrange]; }
#pragma omp section
	{kplist = new int*[maxkeyindexrange]; }
#pragma omp section
	{citeinvlist = new int*[maxpubindexrange]; }
#pragma omp section
	{palist = new int*[maxpubindexrange]; }
#pragma omp section
	{pvlist = new int*[maxpubindexrange]; }
#pragma omp section
	{pklist = new int*[maxpubindexrange]; }
#pragma omp section
	{citenum = new int[maxpubindexrange];
	memset(citenum, 0, maxpubindexrange * sizeof(int)); }
#pragma omp section
	{apnum = new int[maxauthindexrange];
	memset(apnum, 0, maxauthindexrange * sizeof(int)); }
#pragma omp section
	{vpnum = new int[maxvenueindexrange];
	memset(vpnum, 0, maxvenueindexrange * sizeof(int)); }
#pragma omp section
	{kpnum = new int[maxkeyindexrange];
	memset(kpnum, 0, maxkeyindexrange * sizeof(int)); }
#pragma omp section
	{citeinvnum = new int[maxpubindexrange];
	memset(citeinvnum, 0, maxpubindexrange * sizeof(int)); }
#pragma omp section
	{panum = new int[maxpubindexrange];
	memset(panum, 0, maxpubindexrange * sizeof(int)); }
#pragma omp section
	{pvnum = new int[maxpubindexrange];
	memset(pvnum, 0, maxpubindexrange * sizeof(int)); }
#pragma omp section
	{pknum = new int[maxpubindexrange];
	memset(pknum, 0, maxpubindexrange * sizeof(int)); }
	}
	int pos = 0;//TOTALWEIGHTNUM-1까지 초기화함(TOTALWEIGHTNUM위치엔 testzero용 기본배열)
	testpos = 0;
	for (int j = 0; j <= (10 - RANDOMJUMP); j += WEIGHTSIZE)
	{
		for (int k = 0; k <= (10 - RANDOMJUMP); k += WEIGHTSIZE)
		{
			for (int l = 0; l <= (10 - RANDOMJUMP); l += WEIGHTSIZE)
			{
				int m = (10 - RANDOMJUMP) - j - k - l;
				if (m >= 0)
				{
					totalweight[pos].cite = j;
					totalweight[pos].pa = k;
					totalweight[pos].pv = l;
					totalweight[pos].pk = m;
					totalweight[pos].weightpos = pos;

					totalweight[pos].a = k; // 추가변수 for test
					totalweight[pos].v = l;
					totalweight[pos].k = m;

					pos++;
				}
				if (testpos)
				{
					if (j&&k&&l&&m)
					{
						testpos = pos - 1;
					}
				}
			}

		}
	}

}

int loadtotal()
{
#pragma omp parallel for num_threads(filenum)
	for (int i = 0; i < filenum; i++)
	{
		ifstream loadfile;
		string loaddata;
		stringstream loadstream;
		int currentindex;
		long docsize;
		int loadnum;
		int j, k;
		string currentline;


		//시작
		//loadpub
		if (TESTMODE)
		{
			loadfile.open("[test]!pub" + to_string(i + 1) + ".txt");
		}
		else
		{
			loadfile.open("!pub" + to_string(i + 1) + ".txt");
		}
		loadfile.seekg(0, ios::end);//파일 끝으로 이동
		docsize = (long)loadfile.tellg();//현재 위치(숫자)를 저장
		loadfile.seekg(0, ios::beg);//파일 처음으로 이동
		loaddata.resize(docsize);//loaddata를 위 숫자 만큼 할당
		loadfile.read(&loaddata[0], docsize);//loadfile 의 파일을 loaddata변수에 저장
		loadfile.close();//loadfile 닫음 (더 안씀)
		loadstream.clear();//stream 초기화
		loadstream.str(loaddata);//loaddata(string)을 stream에 옮김
		getline(loadstream, currentline);//이제부터 stream에서 뽑아씀
		//로드시작
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());//한 pubx.txt 안에 있는 pub 갯수
		hashpub[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)//loadnum만큼 for문 돌면서 읽음
		{

			getline(loadstream, currentline);//index받아옴
			currentindex = atoi(currentline.c_str());
			hashpub[i][j] = currentindex;
			publist[currentindex].hash = i;
			//이름
			getline(loadstream, publist[currentindex].name);
			//년도
			getline(loadstream, currentline);
			publist[currentindex].year = atoi(currentline.c_str());
			//오버뷰
			getline(loadstream, publist[currentindex].overview);
			//연결갯수
			getline(loadstream, currentline);
			citenum[currentindex] = atoi(currentline.c_str());
			citelist[currentindex] = new int[citenum[currentindex]];
			//연결된것들
			for (k = 0; k < citenum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				citelist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			citeinvnum[currentindex] = atoi(currentline.c_str());
			citeinvlist[currentindex] = new int[citeinvnum[currentindex]];
			//연결된것들
			for (k = 0; k < citeinvnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				citeinvlist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			panum[currentindex] = atoi(currentline.c_str());
			palist[currentindex] = new int[panum[currentindex]];
			//연결된것들
			for (k = 0; k < panum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				palist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			pvnum[currentindex] = atoi(currentline.c_str());
			pvlist[currentindex] = new int[pvnum[currentindex]];
			//연결된것들
			for (k = 0; k < pvnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				pvlist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			pknum[currentindex] = atoi(currentline.c_str());
			pklist[currentindex] = new int[pknum[currentindex]];
			//연결된것들
			for (k = 0; k < pknum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				pklist[currentindex][k] = atoi(currentline.c_str());
			}

			getline(loadstream, currentline);
		}

		loadstream.str("");
		loaddata.clear();
		loadstream.clear();
		//authload

		if (TESTMODE)
		{
			loadfile.open("[test]!auth" + to_string(i + 1) + ".txt");
		}
		else
		{
			loadfile.open("!auth" + to_string(i + 1) + ".txt");
		}
		loadfile.seekg(0, ios::end);
		docsize = (long)loadfile.tellg();
		loadfile.seekg(0, ios::beg);
		loaddata.resize(docsize);
		loadfile.read(&loaddata[0], docsize);
		loadfile.close();
		loadstream.clear();
		loadstream.str(loaddata);
		getline(loadstream, currentline);
		//로드시작
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());
		hashauth[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)
		{

			getline(loadstream, currentline);
			currentindex = atoi(currentline.c_str());
			hashauth[i][j] = currentindex;
			authlist[currentindex].hash = i;
			//이름
			getline(loadstream, authlist[currentindex].name);
			//연결갯수
			getline(loadstream, currentline);
			apnum[currentindex] = atoi(currentline.c_str());
			aplist[currentindex] = new int[apnum[currentindex]];
			//연결된것들
			for (k = 0; k < apnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				aplist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
		}

		loadstream.str("");
		loaddata.clear();
		loadstream.clear();
		//loadvenue
		if (TESTMODE)
		{
			loadfile.open("[test]!venue" + to_string(i + 1) + ".txt");
		}
		else
		{
			loadfile.open("!venue" + to_string(i + 1) + ".txt");
		}
		loadfile.seekg(0, ios::end);
		docsize = (long)loadfile.tellg();
		loadfile.seekg(0, ios::beg);
		loaddata.resize(docsize);
		loadfile.read(&loaddata[0], docsize);
		loadfile.close();
		loadstream.clear();
		loadstream.str(loaddata);
		getline(loadstream, currentline);
		//로드시작
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());
		hashvenue[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)
		{
			getline(loadstream, currentline);
			currentindex = atoi(currentline.c_str());
			hashvenue[i][j] = currentindex;
			venuelist[currentindex].hash = i;
			//이름
			getline(loadstream, venuelist[currentindex].name);
			//연결갯수
			getline(loadstream, currentline);
			vpnum[currentindex] = atoi(currentline.c_str());
			vplist[currentindex] = new int[vpnum[currentindex]];
			//연결된것들
			for (k = 0; k < vpnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				vplist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
		}

		loadstream.str("");
		loaddata.clear();
		loadstream.clear();


		//load key
		if (TESTMODE)
		{
			loadfile.open("[test]!key" + to_string(i + 1) + ".txt");
		}
		else
		{
			loadfile.open("!key" + to_string(i + 1) + ".txt");
		}
		loadfile.seekg(0, ios::end);
		docsize = (long)loadfile.tellg();
		loadfile.seekg(0, ios::beg);
		loaddata.resize(docsize);
		loadfile.read(&loaddata[0], docsize);
		loadfile.close();
		loadstream.clear();
		loadstream.str(loaddata);
		getline(loadstream, currentline);
		//로드시작
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());
		hashkey[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)
		{

			getline(loadstream, currentline);
			currentindex = atoi(currentline.c_str());
			hashkey[i][j] = currentindex;
			keylist[currentindex].hash = i;
			//이름
			getline(loadstream, keylist[currentindex].name);
			//연결갯수
			getline(loadstream, currentline);
			kpnum[currentindex] = atoi(currentline.c_str());
			kplist[currentindex] = new int[kpnum[currentindex]];
			//연결된것들
			for (k = 0; k < kpnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				kplist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
		}
	}
	return 0;
}
void printstatus()
{
	int checknum = 0;
	cout << "======================" << endl;
	if (TESTMODE)
		cout << "!TESTMODE ON!" << endl;
	cout << "current status" << endl;
	cout << "totalpub:" << totalpub << "=";
#pragma omp parallel for reduction(+:checknum)
	for (int i = 0; i < filenum; i++)
	{
		checknum += (int)hashpub[i].size();
	}
	cout << checknum << endl;
	checknum = 0;
	cout << "totalauth:" << totalauth << "=";
#pragma omp parallel for reduction(+:checknum)
	for (int i = 0; i < filenum; i++)
	{
		checknum += (int)hashauth[i].size();
	}
	cout << checknum << endl;
	checknum = 0;
	cout << "totalvenue:" << totalvenue << "=";
#pragma omp parallel for reduction(+:checknum)
	for (int i = 0; i < filenum; i++)
	{
		checknum += (int)hashvenue[i].size();
	}
	cout << checknum << endl;
	checknum = 0;
	cout << "totalkeyword:" << totalkey << "=";
#pragma omp parallel for reduction(+:checknum)
	for (int i = 0; i < filenum; i++)
	{
		checknum += (int)hashkey[i].size();
	}
	cout << checknum << endl;
	cout << "======================" << endl;
}

void pubpagerank(int startindex)
{
	double* beforepub;
	double* afterpub;
	double* beforeauth;
	double* afterauth;
	double* beforevenue;
	double* aftervenue;
	double* beforekey;
	double* afterkey;
	int rankpos[BASELINE];
	double rankvalue[BASELINE];
	int trymax = 9;
	int trymin = 1;
	double stopboarder = 0.001;
	double totalgap;

	double totalusedtopub;
	double totalusedtoauth;
	double totalusedtovenue;
	double totalusedtokey;
	double totalused;
	int trynum;
	int currentweight;
	double dupvalue;//except start value

	studyresult.open("!!pagerankresult" + to_string(startindex) + ".txt");
	studyresult << "current index" << endl;
	studyresult << startindex << endl;
#pragma omp parallel sections
	{
#pragma omp section
	{beforepub = new double[maxpubindexrange]; }
#pragma omp section
	{afterpub = new double[maxpubindexrange]; }
#pragma omp section
	{beforeauth = new double[maxauthindexrange]; }
#pragma omp section
	{afterauth = new double[maxauthindexrange]; }
#pragma omp section
	{beforevenue = new double[maxvenueindexrange]; }
#pragma omp section
	{aftervenue = new double[maxvenueindexrange]; }
#pragma omp section
	{beforekey = new double[maxkeyindexrange]; }
#pragma omp section
	{afterkey = new double[maxkeyindexrange]; }
	}
	//끝

	//각 weight별로 시작
	//for (int currentweight = 0; currentweight < TOTALWEIGHTNUM; currentweight++)
	//for (int currentweight = WITCHWEIGHT; currentweight < WITCHWEIGHT+1; currentweight++)
	for (int k=0; k<1; k++)
	{
#pragma omp parallel sections
	{
#pragma omp section
	{memset(beforepub, 0, maxpubindexrange * sizeof(double)); }
#pragma omp section
	{memset(afterpub, 0, maxpubindexrange * sizeof(double)); }
#pragma omp section
	{memset(beforeauth, 0, maxauthindexrange * sizeof(double)); }
#pragma omp section
	{memset(afterauth, 0, maxauthindexrange * sizeof(double)); }
#pragma omp section
	{memset(beforevenue, 0, maxvenueindexrange * sizeof(double)); }
#pragma omp section
	{memset(aftervenue, 0, maxvenueindexrange * sizeof(double)); }
#pragma omp section
	{memset(beforekey, 0, maxkeyindexrange * sizeof(double)); }
#pragma omp section
	{memset(afterkey, 0, maxkeyindexrange * sizeof(double)); }
#pragma omp section
	{memset(rankpos, -1, BASELINE * sizeof(int)); }
#pragma omp section
	{memset(rankvalue, 0, BASELINE * sizeof(double)); }
	}
	beforepub[startindex] = 1;
	totalgap = 0;
	trynum = 0;

	// 초기 각 노드 점수 출력
	currentweight = 20;
	cout << "currentweight : " << currentweight << endl << endl;

	cout << "trynum : start" << endl;

	for (int i = 0; i < maxpubindexrange; i++) {
		cout << "beforpub[" << i << "] = " << beforepub[i] << endl;
	}
	for (int i = 0; i <maxauthindexrange; i++) {
		cout << "beforauth[" << i << "] = " << beforeauth[i] << endl;
	}
	for (int i = 0; i < maxvenueindexrange; i++) {
		cout << "beforvenue[" << i << "] = " << beforevenue[i] << endl;
	}
	for (int i = 0; i < maxkeyindexrange; i++) {
		cout << "beforkey[" << i << "] = " << beforekey[i] << endl;
	}
	cout << endl << endl;


	while ((totalgap > stopboarder && trynum < trymax) || (trynum < trymin))
	{


		if (trynum < 3)
			currentweight = 14;
		if (trynum >= 3 && trynum < 6)
			currentweight = 34;
		if (trynum >= 6 && trynum < 9)
			currentweight = 14;

		//if (trynum == 0)
		//	currentweight = 14;
		//if (trynum == 1)
		//	currentweight = 34;
		//if (trynum == 2)
		//	currentweight = 14;


		cout << "trynum :" << trynum << "st iteration" << endl << endl;
		cout << "current weight : " << totalweight[currentweight].cite << " " << totalweight[currentweight].pa << " " << totalweight[currentweight].pv << " " << totalweight[currentweight].pk << endl << endl;


		// iteration 전에 각 노드 점수 출력
		for (int i = 0; i < maxpubindexrange; i++) {
			cout << "beforpub[" << i << "] = " << beforepub[i] << endl;
		}
		cout << endl;
		for (int i = 0; i <maxauthindexrange; i++) {
			cout << "beforauth[" << i << "] = " << beforeauth[i] << endl;
		}
		cout << endl;
		for (int i = 0; i < maxvenueindexrange; i++) {
			cout << "beforvenue[" << i << "] = " << beforevenue[i] << endl;
		}
		cout << endl;
		for (int i = 0; i < maxkeyindexrange; i++) {
			cout << "beforkey[" << i << "] = " << beforekey[i] << endl;
		}
		cout << endl;

		totalgap = 0;
		totalused = 0;
		totalusedtopub = 0;
		totalusedtoauth = 0;
		totalusedtovenue = 0;
		totalusedtokey = 0;
		if (1)//trynum)//at start, it's sure no pub and venue jump
		{
			//afterpub 연산
//#pragma omp parallel for reduction(+:totalusedtopub)
			for (int i = 0; i < maxpubindexrange; i++)
			{
				int targetsize, j, target;
				double value;
				targetsize = citeinvnum[i];
				if (targetsize)
				{

					for (j = 0; j < targetsize; j++)
					{
						target = citeinvlist[i][j];
						if (1)//target != startindex) // startindex에서 쏴주는 점수는 처리 안하는 상태 
						{
							value = beforepub[target] * (totalweight[currentweight].cite) / citenum[target];
							value /= 10;//weight 가 10배라서
							if (value)
							{
								afterpub[i] += value;
								totalusedtopub += value;
								cout << "afterpub[" << i << "] get " << value << " from " << target << " pub's cite" << endl;
							}

						}
					}
				}
				targetsize = panum[i];
				if (targetsize)
				{
					for (j = 0; j < targetsize; j++)
					{
						target = palist[i][j];
						value = beforeauth[target] * (totalweight[currentweight].a) / apnum[target]; // totalweight[currentweight].ap -> a
						value /= 10;
						if (value)
						{
							afterpub[i] += value;
							totalusedtopub += value;
							cout << "afterpub[" << i << "] get " << value << " from " << target << " auth" << endl;
						}
					}
				}
				targetsize = pvnum[i];
				if (targetsize)
				{
					for (j = 0; j < targetsize; j++)
					{
						target = pvlist[i][j];
						value = beforevenue[target] * (totalweight[currentweight].v) / vpnum[target]; // totalweight[currentweight].vp -> v
						value /= 10;
						if (value)
						{
							afterpub[i] += value;
							totalusedtopub += value;
							cout << "afterpub[" << i << "] get " << value << " from " << target << " venue" << endl;
						}
					}
				}


				targetsize = pknum[i];
				if (targetsize)
				{
					for (j = 0; j < targetsize; j++)
					{

						target = pklist[i][j];
						value = beforekey[target] * (totalweight[currentweight].k) / kpnum[target]; // totalweight[currentweight].kp -> k
						value /= 10;
						if (value)
						{
							afterpub[i] += value;
							totalusedtopub += value;
							cout << "afterpub[" << i << "] get " << value << " from " << target << " key" << endl;
						}
					}
				}
			}


			//aftervenue 연산
//#pragma omp parallel for reduction(+:totalusedtovenue)
			for (int i = 0; i < maxvenueindexrange; i++)
			{
				int targetsize, j, target;
				double value;
				targetsize = vpnum[i];
				if (targetsize)
				{
					for (j = 0; j < targetsize; j++)
					{
						target = vplist[i][j];
						if (target != startindex)
						{
							value = beforepub[target] * (totalweight[currentweight].pv) / pvnum[target];
							value /= 10;
							if (value)
							{
								aftervenue[i] += value;
								totalusedtovenue += value;
								cout << "aftervenue[" << i << "] get " << value << " from " << target << " pub's venue" << endl;
							}
						}
					}
				}
			}

		}  // if(trynum) 끝


		//afterauth 연산
//#pragma omp parallel for reduction(+:totalusedtoauth)
		for (int i = 0; i < maxauthindexrange; i++)
		{
			int targetsize, j, target;
			double value;
			targetsize = apnum[i];
			if (targetsize)
			{
				for (j = 0; j < targetsize; j++)
				{
					target = aplist[i][j];
					value = beforepub[target] * (totalweight[currentweight].pa) / panum[target];
					value /= 10;
					if (value)
					{
						afterauth[i] += value;
						totalusedtoauth += value;
						cout << "afterauth[" << i << "] get " << value << " from " << target << " pub's auth" << endl;
					}
				}
			}
		}


		//afterkey 연산
//#pragma omp parallel for reduction(+:totalusedtokey)
		for (int i = 0; i < maxkeyindexrange; i++)
		{
			int targetsize, j, target;
			double value;
			targetsize = kpnum[i];
			if (targetsize)
			{
				for (j = 0; j < targetsize; j++)
				{
					target = kplist[i][j];
					value = beforepub[target] * (totalweight[currentweight].pk) / pknum[target];
					value /= 10;
					if (value)
					{
						afterkey[i] += value;
						totalusedtokey += value;
						cout << "afterkey[" << i << "] get " << value << " from " << target << " pub's key" << endl;
					}
				}
			}
		}


		totalused += totalusedtopub;
		totalused += totalusedtoauth;
		totalused += totalusedtovenue;
		totalused += totalusedtokey;


		
		
		cout << "totalusedtopub : " << totalusedtopub << endl;
		cout << "totalusedtoauth : " << totalusedtoauth << endl;
		cout << "totalusedtovenue : " << totalusedtovenue << endl;
		cout << "totalusedtokey : " << totalusedtokey << endl;
		cout << "-jump finished (total used:" << totalused << ")" << endl << endl;

	

		//cout << "-jump finished (total used:" << totalused << ")";
		//나머지 처리 시작
		//afterpub[startindex] += (1 - totalused); //////??????

		//gap 생성 및  처리
		totalgap = 0;
//#pragma omp parallel for reduction(+:totalusedtopub)
		for (int i = 0; i < maxpubindexrange; i++)
		{
			double value = afterpub[i] - beforepub[i];
			if (value > 0)
			{
				totalusedtopub += value;
			}
			beforepub[i] += afterpub[i];
			afterpub[i] = 0;
		}
		totalgap += totalusedtopub;
//#pragma omp parallel for reduction(+:totalusedtoauth)
		for (int i = 0; i < maxauthindexrange; i++)
		{
			double value = afterauth[i] - beforeauth[i];
			if (value > 0)
			{
				totalusedtoauth += value;
			}
			beforeauth[i] += afterauth[i];
			afterauth[i] = 0;
		}
		totalgap += totalusedtoauth;
//#pragma omp parallel for reduction(+:totalusedtovenue)
		for (int i = 0; i < maxvenueindexrange; i++)
		{
			double value = aftervenue[i] - beforevenue[i];
			if (value > 0)
			{
				totalusedtovenue += value;
			}
			beforevenue[i] += aftervenue[i];
			aftervenue[i] = 0;
		}
		totalgap += totalusedtovenue;
//#pragma omp parallel for reduction(+:totalusedtokey)
		for (int i = 0; i < maxkeyindexrange; i++)
		{
			double value = afterkey[i] - beforekey[i]; //////// after는 초기화되므로 거의?무조건? before보다 작을텐데!!확인
			if (value > 0)
			{
				totalusedtokey += value;
			}
			beforekey[i] += afterkey[i];
			afterkey[i] = 0;
		}
		totalgap += totalusedtokey;
		

		// iteration 후에 각 노드 점수 출력
		for (int i = 0; i < maxpubindexrange; i++) {
			cout << "afterpub[" << i << "] = " << beforepub[i] << endl;
		}
		cout << endl;
		for (int i = 0; i <maxauthindexrange; i++) {
			cout << "afterauth[" << i << "] = " << beforeauth[i] << endl;
		}
		cout << endl;
		for (int i = 0; i < maxvenueindexrange; i++) {
			cout << "aftervenue[" << i << "] = " << beforevenue[i] << endl;
		}
		cout << endl;
		for (int i = 0; i < maxkeyindexrange; i++) {
			cout << "afterkey[" << i << "] = " << beforekey[i] << endl;
		}
		cout << endl;

		cout << "totalusedtopub : " << totalusedtopub << endl;
		cout << "totalusedtoauth : " << totalusedtoauth << endl;
		cout << "totalusedtovenue : " << totalusedtovenue << endl;
		cout << "totalusedtokey : " << totalusedtokey << endl;
		//cout << "totalused : " << totalused << endl << endl;

		cout << "totalgap : " << totalgap << endl;
		cout << "----------------------------------------" << endl << endl;
		


		trynum++;

	}//while끝 즉 pagerank 끝

	 //save start
	studyresult << "currentweight" << endl;
	studyresult << currentweight << endl;
	dupvalue = 1;// - beforepub[startindex];
	if (dupvalue)
	{
		studyresult << "dupvalue : ";
		studyresult << dupvalue << endl;
		//order value
		for (int i = 0; i < maxpubindexrange; i++)
		{
			if (i != startindex)
			{
				for (int j = 0; j < BASELINE; j++)
				{
					if (rankvalue[j] < beforepub[i])
					{
						if (j)
						{
							rankvalue[j - 1] = rankvalue[j];
							rankpos[j - 1] = rankpos[j];
						}
						rankpos[j] = i;
						rankvalue[j] = beforepub[i];
					}
					else
					{
						j = BASELINE;
					}
				}
			}
		}




		for (int i = BASELINE - 1; i > -1; i--)
		{
			studyresult << "rankpos[" << i << "] : ";
			studyresult << rankpos[i] << endl;
			if (rankpos[i] == -1)
			{
				i = -1;
			}
			else
			{
				studyresult << "rankvalue[" << i << "]/dupvalue : ";
				studyresult << (rankvalue[i]) / dupvalue << endl;
			}
		}


	}
	else
	{
		studyresult << "dupvalue <= 0 .. ";
		studyresult << -1 << endl;
	}






	}

	delete beforepub;
	delete afterpub;
	delete beforeauth;
	delete afterauth;
	delete beforevenue;
	delete aftervenue;
	delete beforekey;
	delete afterkey;
	studyresult.close();

}

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <omp.h>
#define MAXVIEW 10 //�������� ���� ǥ���� �ִ밹��
#define TESTMODE 1//�������� 0(�� ��ž���� 1)
#define RANDOMJUMP 2//0.2
#define BASELINE 500 //500//500�� �� ���� ���


//#define YEARGAPSIZE 3 //�� �ܰ迡�� year gap �Ⱦ�
//#define YEARGAPNUM 3
#define WEIGHTSIZE 2
#define RESULTNUM 4//weight ž ���� ����
#define TEMPN ((10-RANDOMJUMP)/WEIGHTSIZE)
#define TOTALWEIGHTNUM (TEMPN+1)*(TEMPN+2)*(TEMPN+3) / 6
//randomjump 2, weightsize�� 1�϶� 165
//randomjump 2 weughtsuze�� 2�϶� 35

#define TOTALPATHNUM 3


using namespace std;
//�ʱⰪ�� !!info.txt���� load
int filenum;
int totalpub;
int totalauth;
int totalvenue;
int totalkey;
int maxpubindexrange;
int maxauthindexrange;
int maxvenueindexrange;
int maxkeyindexrange;
int testpos;//��� weight���� 0�̾ƴ� pos �� �ϳ�( ó���� ���ް������� Ȯ���Ҷ� ����)



			//�����͵�
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


////////////�ؽ� ���̺�
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





//�������� ����� ������




//load�� �Լ� �� ������
void init();
int loadtotal();
//�Ϲ��۾�
void printstatus();
//pagerank ����
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

	int a = (10 - RANDOMJUMP); // �߰����� for test
	int k = (10 - RANDOMJUMP);
	int v = (10 - RANDOMJUMP);
}totalweight[TOTALWEIGHTNUM];//10����1 �ؼ� ���ϴ�

class pathclass
{
public:
	int cite = 0;
	int citeinv = (10 - RANDOMJUMP);
	int pa = 0;
	int ap = 0;
	int pathpos = TOTALPATHNUM;
}totalpath[TOTALPATHNUM];

void pubpagerank(int currentindex);
ifstream studytarget;
ofstream studyresult;

///////////////////////////////////����////////////////////////
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
	if (TESTMODE == 0)
		studytarget.open("!!input.txt");
	else
		studytarget.open("[test]!!input.txt");

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
	int pos = 0;//TOTALWEIGHTNUM-1���� �ʱ�ȭ��(TOTALWEIGHTNUM��ġ�� testzero�� �⺻�迭)
	testpos = 0;


	// ��ȵǼ� �밡��
	totalpath[0].pa = (10 - RANDOMJUMP);
	totalpath[1].ap = (10 - RANDOMJUMP);
	totalpath[2].cite = (10 - RANDOMJUMP);

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

					totalweight[pos].a = k; // �߰����� for test
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


		//����
		//loadpub
		if (TESTMODE)
		{
			loadfile.open("[test]!pub" + to_string(i + 1) + ".txt");
		}
		else
		{
			loadfile.open("!pub" + to_string(i + 1) + ".txt");
		}
		loadfile.seekg(0, ios::end);//���� ������ �̵�
		docsize = (long)loadfile.tellg();//���� ��ġ(����)�� ����
		loadfile.seekg(0, ios::beg);//���� ó������ �̵�
		loaddata.resize(docsize);//loaddata�� �� ���� ��ŭ �Ҵ�
		loadfile.read(&loaddata[0], docsize);//loadfile �� ������ loaddata������ ����
		loadfile.close();//loadfile ���� (�� �Ⱦ�)
		loadstream.clear();//stream �ʱ�ȭ
		loadstream.str(loaddata);//loaddata(string)�� stream�� �ű�
		getline(loadstream, currentline);//�������� stream���� �̾ƾ�
										 //�ε����
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());//�� pubx.txt �ȿ� �ִ� pub ����
		hashpub[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)//loadnum��ŭ for�� ���鼭 ����
		{

			getline(loadstream, currentline);//index�޾ƿ�
			currentindex = atoi(currentline.c_str());
			hashpub[i][j] = currentindex;
			publist[currentindex].hash = i;
			//�̸�
			getline(loadstream, publist[currentindex].name);
			//�⵵
			getline(loadstream, currentline);
			publist[currentindex].year = atoi(currentline.c_str());
			//������
			getline(loadstream, publist[currentindex].overview);
			//���᰹��
			getline(loadstream, currentline);
			citenum[currentindex] = atoi(currentline.c_str());
			citelist[currentindex] = new int[citenum[currentindex]];
			//����Ȱ͵�
			for (k = 0; k < citenum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				citelist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			citeinvnum[currentindex] = atoi(currentline.c_str());
			citeinvlist[currentindex] = new int[citeinvnum[currentindex]];
			//����Ȱ͵�
			for (k = 0; k < citeinvnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				citeinvlist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			panum[currentindex] = atoi(currentline.c_str());
			palist[currentindex] = new int[panum[currentindex]];
			//����Ȱ͵�
			for (k = 0; k < panum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				palist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			pvnum[currentindex] = atoi(currentline.c_str());
			pvlist[currentindex] = new int[pvnum[currentindex]];
			//����Ȱ͵�
			for (k = 0; k < pvnum[currentindex]; k++)
			{
				getline(loadstream, currentline);
				pvlist[currentindex][k] = atoi(currentline.c_str());
			}
			getline(loadstream, currentline);
			pknum[currentindex] = atoi(currentline.c_str());
			pklist[currentindex] = new int[pknum[currentindex]];
			//����Ȱ͵�
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
		//�ε����
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());
		hashauth[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)
		{

			getline(loadstream, currentline);
			currentindex = atoi(currentline.c_str());
			hashauth[i][j] = currentindex;
			authlist[currentindex].hash = i;
			//�̸�
			getline(loadstream, authlist[currentindex].name);
			//���᰹��
			getline(loadstream, currentline);
			apnum[currentindex] = atoi(currentline.c_str());
			aplist[currentindex] = new int[apnum[currentindex]];
			//����Ȱ͵�
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
		//�ε����
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());
		hashvenue[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)
		{
			getline(loadstream, currentline);
			currentindex = atoi(currentline.c_str());
			hashvenue[i][j] = currentindex;
			venuelist[currentindex].hash = i;
			//�̸�
			getline(loadstream, venuelist[currentindex].name);
			//���᰹��
			getline(loadstream, currentline);
			vpnum[currentindex] = atoi(currentline.c_str());
			vplist[currentindex] = new int[vpnum[currentindex]];
			//����Ȱ͵�
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
		//�ε����
		getline(loadstream, currentline);
		loadnum = atoi(currentline.c_str());
		hashkey[i].resize(loadnum);
		for (j = 0; j < loadnum; j++)
		{

			getline(loadstream, currentline);
			currentindex = atoi(currentline.c_str());
			hashkey[i][j] = currentindex;
			keylist[currentindex].hash = i;
			//�̸�
			getline(loadstream, keylist[currentindex].name);
			//���᰹��
			getline(loadstream, currentline);
			kpnum[currentindex] = atoi(currentline.c_str());
			kplist[currentindex] = new int[kpnum[currentindex]];
			//����Ȱ͵�
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
	int pubrank[BASELINE];
	double rankresult[BASELINE];
	int trymax = 4;
	int trymin = 2;
	double stopboarder = 0.001;
	double totalgap;

	double totalusedtopub;
	double totalusedtoauth;
	double totalusedtovenue;
	double totalusedtokey;
	double totalused;
	int trynum;
	int currentweight;
	int currentpath;
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
	//��

	//�� weight���� ����
	//for (int currentweight = 0; currentweight < TOTALWEIGHTNUM; currentweight++)
	//for (int currentweight = WITCHWEIGHT; currentweight < WITCHWEIGHT+1; currentweight++)
	for (int k = 0; k<1; k++)
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
	{memset(pubrank, -1, BASELINE * sizeof(int)); }
#pragma omp section
	{memset(rankvalue, 0, BASELINE * sizeof(double)); }
#pragma omp section
	{memset(rankresult, 0, BASELINE * sizeof(double)); }
	}

	beforepub[startindex] = 1;
	totalgap = 0;
	trynum = 0;
	currentpath = 0;
	currentweight = 34; // 8 0 0 0  to ignore weight


	while ((totalgap > stopboarder && trynum < trymax) || (trynum < trymin))
	{
		// PA - AP - PP - PA
		if (trynum == 0)
			currentpath = 0; // PA
		if (trynum == 1)
			currentpath = 1; // AP
		if (trynum == 2)
			currentpath = 2;  // PP
		if (trynum == 3)
			currentpath = 0; // PA

		cout << "trynum :" << trynum << "st iteration" << endl << endl;
		cout << "current path : " << totalpath[currentpath].pa << " " << totalpath[currentpath].ap << " " << totalpath[currentpath].cite << endl << endl;


		totalgap = 0;
		totalused = 0;
		totalusedtopub = 0;
		totalusedtoauth = 0;
		totalusedtovenue = 0;
		totalusedtokey = 0;
		if (1)//trynum)//at start, it's sure no pub and venue jump
		{
			//afterpub ����
#pragma omp parallel for reduction(+:totalusedtopub)
			for (int i = 0; i < maxpubindexrange; i++)    //// pub ������ �޾ƿ�����.. 
			{
				int targetsize, j, target;
				int yearGap;
				double value;
				targetsize = citeinvnum[i];
				if (targetsize)
				{

					for (j = 0; j < targetsize; j++)
					{
						target = citeinvlist[i][j];
						yearGap = publist[target].year - publist[i].year;
						///if (target != startindex) // ����Pub������ cite ���� ���ϰ� �ϴºκ�!

						if (target == startindex) {
							if (yearGap > 2) { // yeargap 2�� �̻� 
								value = beforepub[target] * (totalpath[currentpath].cite) / citenum[target];// PP
								value /= 10;//weight �� 10���..
								if (value)
								{
									afterpub[i] += value;
									totalusedtopub += value;
									cout << "afterpub[" << i << "] get " << value << " from " << target << " pub's cite(startindex)" << endl;
									//cout << "yearGap : " << yearGap << endl;
								}
							}
						}
						else {
							value = beforepub[target] * (totalpath[currentpath].cite) / citenum[target]; // PP
							value /= 10;//weight �� 10���..
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
						value = beforeauth[target] * (totalpath[currentpath].ap);// ���� �ȳ����� �й�!(@����ġȭ �ؾ���) / apnum[target]; // AP
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
						value = 0;// beforevenue[target] * (totalweight[currentweight].v) / vpnum[target]; // totalweight[currentweight].vp -> v
						value /= 10;
						if (value)
						{
							afterpub[i] += value;
							totalusedtopub += value;
							//cout << "afterpub[" << i << "] get " << value << " from " << target << " venue" << endl;
						}
					}
				}


				targetsize = pknum[i];
				if (targetsize)
				{
					for (j = 0; j < targetsize; j++)
					{

						target = pklist[i][j];
						value = 0;// beforekey[target] * (totalweight[currentweight].k) / kpnum[target]; // totalweight[currentweight].kp -> k
						value /= 10;
						if (value)
						{
							afterpub[i] += value;
							totalusedtopub += value;
							//cout << "afterpub[" << i << "] get " << value << " from " << target << " key" << endl;
						}
					}
				}
			}									//// ������� pub ������ �޾ƿ��� �Ϸ�.


												//aftervenue ����
#pragma omp parallel for reduction(+:totalusedtovenue)
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
							value = 0;// beforepub[target] * (totalweight[currentweight].pv) / pvnum[target];
							value /= 10;
							if (value)
							{
								aftervenue[i] += value;
								totalusedtovenue += value;
								//cout << "aftervenue[" << i << "] get " << value << " from " << target << " pub's venue" << endl;
							}
						}
					}
				}
			}

		}  // if(trynum) ��


		   //afterauth ����
#pragma omp parallel for reduction(+:totalusedtoauth)
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
					value = beforepub[target] * (totalpath[currentpath].pa);// ���⵵(@����ġȭ) / panum[target];  // PA
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


		//afterkey ����
#pragma omp parallel for reduction(+:totalusedtokey)
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
					value = 0;// beforepub[target] * (totalweight[currentweight].pk) / pknum[target];
					value /= 10;
					if (value)
					{
						afterkey[i] += value;
						totalusedtokey += value;
						//cout << "afterkey[" << i << "] get " << value << " from " << target << " pub's key" << endl;
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


		//������ ó�� ����

		//if (trynum == 0) {
		//	beforepub[startindex] = 0;
		//	afterpub[startindex] += (1 - totalused); // // �켱 ù��° itation�� ����pub���� ���� �ѷ��ְ� ���� �� �Է�.
		//												// Because ��� �ִٺ��� totalused �ʹ�Ŀ���� ������ �߻�. 
		//}

		//afterpub[startindex] += (1 - totalused);   // @����ġȭ �ϸ� �������� randomJump �ʿ������.

		//gap ���� ��  ó��
		totalgap = 0;
#pragma omp parallel for reduction(+:totalusedtopub)
		for (int i = 0; i < maxpubindexrange; i++)
		{
			double value = afterpub[i] - beforepub[i];
			if (value > 0)
			{
				totalusedtopub += value;
			}
			beforepub[i] = afterpub[i];
			afterpub[i] = 0;
		}
		totalgap += totalusedtopub;
#pragma omp parallel for reduction(+:totalusedtoauth)
		for (int i = 0; i < maxauthindexrange; i++)
		{
			double value = afterauth[i] - beforeauth[i];
			if (value > 0)
			{
				totalusedtoauth += value;
			}
			beforeauth[i] = afterauth[i];
			afterauth[i] = 0;
		}
		totalgap += totalusedtoauth;
#pragma omp parallel for reduction(+:totalusedtovenue)
		for (int i = 0; i < maxvenueindexrange; i++)
		{
			double value = aftervenue[i] - beforevenue[i];
			if (value > 0)
			{
				totalusedtovenue += value;
			}
			beforevenue[i] = aftervenue[i];
			aftervenue[i] = 0;
		}
		totalgap += totalusedtovenue;
#pragma omp parallel for reduction(+:totalusedtokey)
		for (int i = 0; i < maxkeyindexrange; i++)
		{
			double value = afterkey[i] - beforekey[i];
			if (value > 0)
			{
				totalusedtokey += value;
			}
			beforekey[i] = afterkey[i];
			afterkey[i] = 0;
		}
		totalgap += totalusedtokey;



		cout << "totalusedtopub : " << totalusedtopub << endl;
		cout << "totalusedtoauth : " << totalusedtoauth << endl;
		cout << "totalusedtovenue : " << totalusedtovenue << endl;
		cout << "totalusedtokey : " << totalusedtokey << endl;

		cout << "totalgap : " << totalgap << endl;
		cout << "----------------------------------------" << endl << endl;



		trynum++;

	}//while�� �� pagerank ��

	int yeargap;
	//save start
	currentweight = 14;
	studyresult << "currentweight" << endl;
	studyresult << currentweight << endl;
	dupvalue = 1;// - beforepub[startindex];  // �������� ���� ���� �������� �������ֵ� �����Ͽ� ���� 1�� ������ֱ� ����.
	if (dupvalue)
	{
		//studyresult << "dupvalue : ";
		studyresult << dupvalue << endl;
		//order value
		for (int i = 0; i < maxauthindexrange; i++)
		{
			//cout << "beforeauth[" << i << "] = " << beforeauth[i] << endl;
			if (1)//i != startindex)
			{
				for (int j = 0; j < BASELINE; j++) //BASELINE �� ��ŭ rank�迭�� ���������س�����.
				{

					if (rankvalue[j] < beforeauth[i])
					{
						if (j)
						{
							rankvalue[j - 1] = rankvalue[j];
							rankpos[j - 1] = rankpos[j];
						}
						rankpos[j] = i;
						rankvalue[j] = beforeauth[i];
					}
					else
					{
						j = BASELINE;
					}
				}
			}
		}
		cout << "yearcheck " << endl;
		cout << "startindex : " << startindex << " year : " << publist[startindex].year << endl;


		for (int i = BASELINE - 1; i > -1; i--) // 100���� ��������? Ȯ��-> 
												// 0~2year�� ����ϵ��� ->
		{
			/*cout << "auth " << rankpos[i] << "wirte these pubs" << endl;
			for (int p = 0; p < apnum[rankpos[i]]; p++) {
			cout << aplist[rankpos[i]][p] << endl;
			}
			cout << "------" << endl;*/

			for (int k = 0; k < apnum[rankpos[i]]; k++) {

				//yeargap = publist[startindex].year - publist[aplist[rankpos[i]][k]].year;

				//if (yeargap >= 0 && yeargap < 3) {
				if(1){
					studyresult << aplist[rankpos[i]][k] << endl;
					if (rankpos[i] == -1) {
						i = -1;
						break;
					}
					else {
						studyresult << rankvalue[i] << endl;
					}
				}

			}

		}


		//for (int i = BASELINE - 1; i > -1; i--)
		//{
		//	//studyresult << "rankpos[" << i << "] : ";
		//	studyresult << pubrank[i] << endl;
		//	if (rankpos[i] == -1)
		//	{
		//		i = -1;
		//	}
		//	else
		//	{
		//		//studyresult << "rankvalue[" << i << "]/dupvalue : ";
		//		studyresult << (rankresult[i]) / dupvalue << endl;
		//	}
		//}


	}
	else
	{
		// points are all in startindex
		//studyresult << "dupvalue <= 0 .. ";
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
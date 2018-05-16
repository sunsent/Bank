
#include <iostream>
#include<fstream>
#include <string>
using namespace std;



const char* Available_file = "Available.dat";//可用资源向量, Available, 向量长度m,Available[j]为资源Rj的可用数量 ；
const char* Max_file = "Max.dat";//最大需求矩阵，Max, n行m列,Max[i][j]表示进程Pi对资源Rj的最大需求量
const char* Allocation_file = "Allocation.dat";//分配矩阵，Allocation, n行m列,Allocation[i][j]表示进程Pi已分到资源Rj数量
const char* Need_file = "Need.dat";//需求矩阵，Need, n行m列,Need[i][j]表示进程Pi对资源Rj的需求数量
const char* Request_file = "Request.dat";//资源请求向量矩阵，Request[i], 向量长度m,Request[i][j]表示进程Pi对资源Rj的请求数量
const char* Initi_file = "Initi.dat";//初始条件:进程数量,资源种类数

class Bank {
public:
	Bank();
	~Bank();
	void initi();
	void readFile(int pocess_sum, int resource_sum, int matrix[10][10], const char* file);
	void readFile(int resource_sum, int matrix[10], const char* file);
	void readIniti_file(const char* file);
	void setPocess_sum(int sum);
	void setResource_sum(int sum);
	void setRequest_T0(int sum);
	
	int getPocess_sum();
	int getSafe_queue(int m);
	int checkCurrentPocess(int currentPocess);
	int checkAll(int _NextLocation);

private:
	int available[10];//可用资源向量
	int max[10][10];//最大需求矩阵
	int allocation[10][10];//分配矩阵
	int need[10][10];//需求矩阵
	int request[10][10];//资源请求向量矩阵
	int queue[10];//执行队列
	int is_pocess_finished[10];//进程是否被满足
	int is_recur_finished[10];//第n层递归是否找到安全进程

	int request_T0;//T0时刻先分配给谁
	int resourceBuffer[10];//当前要分配的资源及数量,现在只用于T0时刻的请求
	int pocess_sum;//进程总数
	int resource_sum;//资源总数 
	int safeQueue[10];//安全队列
};

int Bank::getPocess_sum() {
	return pocess_sum;
}

int Bank::getSafe_queue(int m) {
	return safeQueue[m];
}

void Bank::readIniti_file(const char* file) {//进程总数,资源总数 ,T0时刻的请求
	ifstream input_file;
	input_file.open(file, ios_base::in);
	if (input_file.is_open()) {
		input_file >> pocess_sum >> resource_sum>>request_T0;
		if (request_T0 > -1) {
			for (int i = 0; i < resource_sum; i++) {
				input_file >> resourceBuffer[i];
			}
		}
		
	}
}

void Bank::readFile(int pocess_sum, int resource_sum, int matrix[10][10], const char* file) {
	ifstream input_file;
	input_file.open(file, ios_base::in);
	if (input_file.is_open()) {
		for (int i = 0; i < pocess_sum; i++) {
			for (int j = 0; j < resource_sum; j++) {
				input_file >> matrix[i][j];
			}
		}

	}
	input_file.close();
}

void Bank::readFile(int resource_sum, int matrix[10], const char* file) {
	ifstream input_file;
	input_file.open(file, ios_base::in);
	if (input_file.is_open()) {
		for (int i = 0; i < resource_sum; i++) {
			input_file >> matrix[i];
		}

	}
	input_file.close();
}

Bank::Bank(){}
Bank::~Bank(){}

void Bank::initi() {
	pocess_sum = 5;
	resource_sum = 3;
	request_T0 = -1;//T0时刻没有特殊要求

	readIniti_file(Initi_file);
	readFile(resource_sum, available, Available_file);
	readFile(pocess_sum, resource_sum, max, Max_file);
	readFile(pocess_sum, resource_sum, allocation, Allocation_file);
	readFile(pocess_sum, resource_sum, need, Need_file);
	readFile(pocess_sum, resource_sum, request, Request_file);
	for (int i = 0; i < 10; i++) {
		is_pocess_finished[i] = 0;
		is_recur_finished[i] = 0;
		safeQueue[i] = -1;
	}

}

//0:请求数多于可用数,1:可用数多于请求数,2:请求数多于需求数
int Bank::checkCurrentPocess(int currentPocess) {
	for (int i = 0; i < resource_sum; i++) {
		if (request[currentPocess][i] > need[currentPocess][i]) return 2;//很有可能request=need
		else {
			if (request[currentPocess][i] > available[i]) return 0;
			else if (request[currentPocess][i] <= available[i]) return 1;
		}
	}
}

int Bank::checkAll(int recursive = 0) {//recursive:递归层数
	int available_temp[10];//使用temp以便在需要时恢复数据
	int allocation_temp[10][10];
	int need_temp[10][10];
	int request_temp[10][10];

	//初始请求-------------------------------
	if (request_T0 > -1) {
		int giveResBack = 0;//如果该进程被满足,就要释放资源,如果没有就不需要
		for (int i = 0; i < resource_sum; i++) {
			//if (resourceBuffer[i] > available[i]) 
			if (resourceBuffer[i] > need[request_T0][i]) {
				cout << "初始请求数多于需求数" << endl;
				return 2;
			}
			else {
				if (resourceBuffer[i] > available[i]) return 0;
				else if (resourceBuffer[i] <= available[i]) {
					// available_temp[i] = available[i] - resourceBuffer[i];
					// allocation_temp[request_T0][i] = allocation[request_T0][i] + resourceBuffer[i];
					// need_temp[request_T0][i] = need[request_T0][i] - resourceBuffer[i];

					available[i] = available[i] - resourceBuffer[i];
					allocation[request_T0][i] = allocation[request_T0][i] + resourceBuffer[i];
					need[request_T0][i] = need[request_T0][i] - resourceBuffer[i];
					request[request_T0][i] = request[request_T0][i] - resourceBuffer[i];
					if (need[request_T0][i] == 0) {
						giveResBack = giveResBack + 1;
					}
				}
			}
			if (giveResBack== resource_sum) {
				//该进程被满足,要释放资源
				for (int res2 = 0; res2 < resource_sum; res2++) {
					available[res2] = available[res2]+max[request_T0][res2];
				}
			}
		}
		request_T0 = -1;//之后的递归没有初始条件,因为初始条件已经被满足
	}
	//--------------------------------------------
	//寻找安全的进程队列
	int flag = -1;
	for (int i = 0; i < pocess_sum; i++) {
		if (is_pocess_finished[i]) {}
		else {
			int checkResult = checkCurrentPocess(i);
			if (checkResult == 1) {
				for (int j = 0; j < resource_sum; j++) {
					//此时的request=need,除了T0时刻提出条件的那个进程
					// available_temp[j] = available[i] - need[i][j];
					// allocation_temp[i][j] = allocation[i][j] + need[i][j];
					// need_temp[i][j] = need[i][j] - need[i][j];
					available_temp[j] = available[j];
					allocation_temp[i][j] = allocation[i][j];
					need_temp[i][j] = need[i][j];
					request_temp[i][j] = request[i][j];

					available[j] = available[j] + max[i][j];//进程被满足后释放资源
					allocation[i][j] = allocation[i][j] - need[i][j];
					need[i][j] = need[i][j] - need[i][j];
					request[i][j] = request[i][j] - request[i][j];

					is_pocess_finished[i] = 1;
					is_recur_finished[recursive] = 1;
					if (recursive >= 1) {
						is_recur_finished[recursive - 1] = 1;
					}

				}
				//recursive=recursive+1;
				if (recursive<pocess_sum - 1) {
					checkAll(recursive + 1);
					if (is_recur_finished[recursive] == 1) {
						safeQueue[recursive] = i;//安全队列第recursive个位置是进程i
						return 1;
					}
					if (is_recur_finished[recursive] == 0) {//找不到下一个安全进程
														   //
						is_pocess_finished[i] = 0;//该进程的所有操作被撤回
						for (int res2 = 0; res2 < resource_sum; res2++) {
							available[res2] = available_temp[res2];
							allocation[i][res2] = allocation_temp[i][res2];
							need[i][res2] = need_temp[i][res2];
							request[i][res2] = request_temp[i][res2];
						}
						
					}
				}
				else if (recursive == pocess_sum - 1) {
					safeQueue[recursive] = i;
					return 1;
					//递归结束,且找到安全队列
				}
				else if(recursive>pocess_sum - 1) {
					cout << "递归出错" << endl;
					return 3;//递归出错
				}


			}
			else {
				if (recursive >= 1) is_recur_finished[recursive - 1] = 0;
			}
		}

	}
	return 4;//没有找到安全进程

}

int main() {
	Bank bank_test;
	bank_test.initi();
	int result = bank_test.checkAll(0);
	int pocess_sum = bank_test.getPocess_sum();
	if (result == 1) {
		cout << "安全队列:" << endl;
		for (int i = 0; i < pocess_sum; i++) {
			cout << "  进程P" << bank_test.getSafe_queue(i) << "   ";
		}
	}
	else {
		cout << "无安全队列" << endl;
	}
	int end;
	cin >> end;
	return 0;
}


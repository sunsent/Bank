
#include "bank.cpp"

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
	return 0;
}
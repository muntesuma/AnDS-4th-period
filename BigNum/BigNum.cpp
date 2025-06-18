#include <iostream>	// библиотека для работы с вводом/выводом в стандартном потоке
#include <ctime>	// библиотека для для работы с временем и датой
#include <random>	// библиотека для генерации случайных чисел
#include <cstring>	// библиотека для работы с символьными массивами
#include <algorithm>	// библиотека для работы с коллекциями данных

using namespace std;	// для работы с пространством имен std  

typedef unsigned char BASE;
typedef unsigned short DBASE;
#define BASE_SIZE (sizeof(BASE) * 8)
#define BASENUM ((DBASE)1 << BASE_SIZE)

mt19937 rnd(time(0));

class BigNumber {	// класс БЧ
private:
	BASE* coef;
	int len;
	int maxlen;

	BigNumber shift(int x) {	// Сдвиг БЧ
		int m = len;
		int n = len + x;
		BigNumber bn(n);
		bn.len = n;
		for (int i = 0; i < m; i++) bn.coef[n - 1 - i] = coef[m - 1 - i];
		while (bn.coef[bn.len - 1] == 0 && (bn.len > 1)) bn.len--;
		return bn;
	}

public:
	BigNumber(int m = 1, bool f = 0) : maxlen(m), len(1) {			// Конструктор БЧ (по умолчанию / с PRNG)
		coef = new BASE[maxlen]{};
		if (!f) for (int i = 0; i < maxlen; coef[i] = 0, i++);
		else {
			for (int i = 0; i < maxlen; coef[i] = rnd(), i++);
			len = maxlen;
			while (coef[len - 1] == 0) coef[len - 1] = rnd();
		}
	}
	BigNumber(const BigNumber& x) : len(x.len), maxlen(x.maxlen) {		// Конструктор копирования
		coef = new BASE[maxlen]{};
		for (int i = 0; i < len; coef[i] = x.coef[i], i++);
	}
	~BigNumber() {								// Деструктор
		delete[] coef;
	}

	void len_norm() {	// Функция для нормализации длины
		len = maxlen;
		while (len > 0 && coef[len - 1] == 0) len--;
		if (len == 0) len = 1;
	}

	BigNumber& operator = (const BigNumber& x) {	// Перегрузка оператора (=) присваивания 
		if (this != &x) {
			if (coef) delete[] coef;
			len = x.len;
			maxlen = x.maxlen;
			coef = new BASE[maxlen]{};
			for (int i = 0; i < len; i++) coef[i] = x.coef[i];
		}
		return *this;
	}

	void input_hex() {	// Ввод hex числа
		char s[256];
		cin >> s;

		int j = 0;
		int k = 0;

		unsigned int tmp = 0;

		this->len = (strlen(s) - 1) / (BASE_SIZE / 4) + 1;	// Рассчет длины большого числа 
		this->maxlen = len;					// Установка максимальной длинны
		this->coef = new BASE[this->maxlen];		// Выделение памяти

		for (int i = 0; i < this->len; i++) this->coef[i] = 0; 	// Инициализация нулями

		for (int i = strlen(s) - 1; i >= 0; i--) {
			if ('0' <= s[i] && s[i] <= '9') tmp = s[i] - '0';		// Преобразование чисел от 0 до 9 в 10-чный вариант
			else if ('a' <= s[i] && s[i] <= 'f') tmp = s[i] - 'a' + 10;	// Преобразование чисел от a до f в 10-чный вариант
			else if ('A' <= s[i] && s[i] <= 'F') tmp = s[i] - 'A' + 10;	// Аналогично для чисел от A до F 
			else {				// Ошибка
				cout << "error";
				exit(0);		// Завершение программы
			}

			this->coef[j] |= tmp << (k * 4);	// Побитовое добавление числа в элемент массива и сдвиг
			k++;
			if (k >= BASE_SIZE / 4) {		// Переход к следующему элементу в случае заполнения
				k = 0;
				j++;
			}
		}
	}
	void print_hex() {					// Вывод hex числа 
		cout << hex << (int)this->coef[len - 1]; 	// Вывод последнего эл-та hex

		for (int j = this->len - 2; j >= 0; j--) {	// Вывод hex числа, начиная с предпоследнего эл-та и заканчивая первым
			cout.width(BASE_SIZE / 4);		// Устанавка ширины поля вывода
			cout.fill('0');				// Устанавка символа заполнения для вывода
			cout << hex << (int)this->coef[j];	// Вывод эл-тов в hex с приведением к int
		}
	}

	bool operator > (const BigNumber& bn) const {		// Перегрузка оператора (>) больше 
		if (len != bn.len) return len > bn.len;		// Проверка по длинне объектов 
		for (int i = len - 1; i >= 0; i--) {		// Посимвольное сравнение, начиная со старших разрядов (с конца)
			if (coef[i] != bn.coef[i]) {
				return coef[i] > bn.coef[i];
			}
		}
		return 0;					// Вывод 0 если значения равны
	}
	bool operator < (const BigNumber& bn) const {		// Перегрузка оператора (<) меньше 
		return (bn > *this);
	}
	bool operator >= (const BigNumber& bn) const {		// Перегрузка оператора (>=) больше или равно
		return !(*this < bn);
	}
	bool operator <= (const BigNumber& bn) const {		// Перегрузка оператора (<=) меньше или равно
		return !(*this > bn);
	}
	bool operator == (const BigNumber& bn) const {		// Перегрузка оператора (==) равенства
		return (*this <= bn && *this >= bn);
	}
	bool operator != (const BigNumber& bn) const {		// Перегрузка оператора (!=) неравенства
		return !(*this == bn);
	}

	BigNumber operator + (const BigNumber& x) {		// Перегрузка оператора (+) сложения (двух БЧ)
		int l = max(this->len, x.len) + 1;		// Определенеие длины результирующего объекта sum
		int t = min(this->len, x.len);		// Определенеие минимальной из длин двух входных объектов
		BigNumber sum(l);				// Объект содержащий результат сложения
		sum.len = l;

		int j = 0;
		int k = 0;
		DBASE tmp;	// Буфер для временного хранения результатов вычислений

		while (j < t) {							// Сложение коэффициентов из двух объектов пока j < t
			tmp = (DBASE)coef[j] + (DBASE)x.coef[j] + (DBASE)k;
			sum.coef[j] = (BASE)tmp;				// Запись значения коэфициента
			k = (BASE)(tmp >> BASE_SIZE);				// Запись переноса, если такой есть
			j++;
		}
		while (j < this->len) {					// Сложение коэффициентов из двух объектов пока j < текущего объекта
			tmp = (DBASE)coef[j] + (DBASE)k;
			sum.coef[j] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			j++;
		}
		while (j < x.len) {						// Сложение коэффициентов из двух объектов пока j < x.len
			tmp = (DBASE)x.coef[j] + (DBASE)k;
			sum.coef[j] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			j++;
		}

		sum.coef[j] = k;			// Запись последнего переноса
		if (sum.coef[j] == 0) sum.len--;	// Удаление ведущего нуля, если такой есть
		return sum;				// Возврат суммы
	}
	BigNumber& operator += (const BigNumber& x) {	// Перегрузка оператора (+=) прибавления
		*this = *this + x;
		return *this;
	}
	BigNumber operator - (const BigNumber& x) {	// Перегрузка оператора (-) вычитания
		DBASE tmp;
		int n = len;
		int m = x.len;
		int i = 0;
		int k = 0;
		BigNumber result(n);			// Результат операции вычитания
		result.len = n;

		if (x > *this) exit(2);		// Проверка на дурака (предусмотрены только положительные числа)

		while (i < m) {						// Вычитание коэффициентов объектов, начиная с младших разрядов
			tmp = ((DBASE)1 << BASE_SIZE) | coef[i];	// Инициализация с учетом доп разряда
			tmp = tmp - x.coef[i] - k;			// Вычитание между текущим коэффициентом текущего объекта и коэффициентом объекта x (с учетом займа)
			result.coef[i] = (BASE)tmp;			// Запись результата
			k = !(tmp >> BASE_SIZE);			// Запись займа
			i++;
		}
		while (i < n) {						// Вычитание оставшихся коэффициентов объекта *this
			tmp = ((DBASE)1 << BASE_SIZE) | coef[i];
			tmp -= k;					// Вычитание между текущим коэффициентом текущего объекта и займом k
			result.coef[i] = (BASE)tmp;
			k = !(tmp >> BASE_SIZE);
			i++;
		}
		while (result.coef[result.len - 1] == 0 && (result.len > 1))	// Удаление ведущих нулей, если такие есть
			result.len--;

		return result;						// Возврат результата разности
	}
	BigNumber& operator -= (const BigNumber& bn) {		// Перегрузка оператора (-=) убавления	
		*this = *this - bn;
		return *this;
	}
	BigNumber operator * (const BASE& num) {			// Перегрузка оператора (*) умножения (на обычное число)	
		DBASE tmp;
		int i = 0;
		int n = len + 1;					// Длинна с учетом заема 1 разряда
		BASE k = 0;
		BigNumber mul(n);
		mul.len = n;

		while (i < n - 1) {					// Покоэфициентное умножение с учетом заема
			tmp = (DBASE)coef[i] * (DBASE)num + (DBASE)k;
			mul.coef[i] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			i++;
		}
		mul.coef[i] = k;					// Запись последнего разряда
		while (mul.coef[mul.len - 1] == 0 && (mul.len > 1)) 	// Удаление ведущих нулей, если требуется
			mul.len--;

		return mul;						// Возврат результата умножения
	}
	BigNumber operator * (const BigNumber& bn) {			// Перегрузка оператора (*) умножения (двух БЧ)
		DBASE tmp;
		int n = len;
		int m = bn.len;
		int l = n + m;						// Длинна с учетом максимального заема разрядов
		BigNumber mul(l);
		mul.len = l;
		int j = 0;

		while (j < m) {
			if (bn.coef[j] != 0) {
				int i = 0;
				BASE k = 0;

				while (i < n) {
					tmp = (DBASE)coef[i] * (DBASE)bn.coef[j] + (DBASE)mul.coef[i + j] + (DBASE)k;
					mul.coef[i + j] = (BASE)tmp;	// Запись результата
					k = (BASE)(tmp >> BASE_SIZE);
					i++;
				}
				mul.coef[j + n] = (BASE)k;		// Запись заема
			}
			j++;
		}
		while (mul.coef[mul.len - 1] == 0 && (mul.len > 1)) mul.len--;

		return mul;
	}
	BigNumber& operator *= (const BigNumber& bn) {		// Перегрузка оператора (*=) умножения текущего объекта
		*this = *this * bn;
		return *this;
	}
	BigNumber operator + (const BASE& num) {		// Перегрузка оператора (+) сложения (с обычным числом)
		DBASE tmp;
		int n = len + 1;
		int i = 0;
		int k = 0;
		BigNumber sum(n);
		sum.len = n;

		tmp = (DBASE)coef[i] + (DBASE)num;		// Основное вычисление
		sum.coef[i] = (BASE)tmp;
		k = (BASE)(tmp >> BASE_SIZE);
		i++;

		while (i < n - 1) {				// Вычисление заемов
			tmp = (DBASE)coef[i] + (DBASE)k;
			sum.coef[i] = (BASE)tmp;
			k = (BASE)(tmp >> BASE_SIZE);
			i++;
		}
		sum.coef[i] = k;
		while (sum.coef[sum.len - 1] == 0 && (sum.len > 1)) sum.len--;

		return sum;					// Возврат суммы
	}
	BigNumber operator / (const BASE& num) { 		// Перегрузка оператора (/) деления (на обычное число)
		if (num == 0) exit(3);				// Проверка на дурака

		DBASE tmp;
		int n = len;
		int i = 0;
		BASE r = 0;
		BigNumber div(n);
		div.len = n;

		while (i < n) {
			tmp = ((DBASE)r << BASE_SIZE) + (DBASE)coef[n - i - 1];
			div.coef[n - i - 1] = (BASE)(tmp / num);
			r = (BASE)(tmp % num);			// Остаток от деления
			i++;
		}
		while (div.coef[div.len - 1] == 0 && (div.len > 1)) div.len--;

		return div;					// Возврат результата деления
	}
	BASE operator % (const BASE& num) {			// Перегрузка оператора (%) деления с остатком (на обычное число)
		if (num == 0) exit(3);

		DBASE tmp;
		int n = len;
		int i = 0;
		BASE r = 0;

		while (i < n) {
			tmp = ((DBASE)r << BASE_SIZE) + (DBASE)coef[n - i - 1];
			r = (BASE)(tmp % num);
			i++;
		}

		return r;					// Возврат результата деления с остатком
	}
	BigNumber operator / (const BigNumber& bn) {		// Перегрузка оператора (/) деления (на БЧ)
		BigNumber e;

		if (bn == e) exit(10);			// Проверка на 0
		if (*this == bn) return (e + 1);	// Проверка на равенство
		if (*this < bn) return e;		// Проверка на дурака
		if (bn.len < 2) {			// Если длинна делителя меньше 2 
			BASE x;
			x = bn.coef[0];
			BigNumber u(*this);
			u = u / x;
			return u;			// Возврат результата деления
		}

		int t = len; 		// Длинна текущего объекта
		int n = bn.len; 	// Длинна делителя
		int m = t - n; 		// Длинна остатка
		int i = 0;
		int d = 0;

		BigNumber u(*this);	// Инициализация для текущего объекта
		BigNumber v(bn);	// Инициализация для делителя
		BigNumber q(m + 1);	// Инициализация для результата деления

		q.len = m + 1;

		DBASE qx = 0;
		DBASE rx = 0;

		d = BASENUM / (v.coef[n - 1] + 1);	// Вычисление для нормализации делителя

		u = u * d;
		v = v * d;
		i = m;

		while (i >= 0) {
			qx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) / v.coef[n - 1];				// Вычисление оценки частного 
			rx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) % v.coef[n - 1];				// Вычисление остатка 

			if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {		// Коррекция оценки частного 
				qx--;
				rx += v.coef[n - 1];
			}
			if (rx < BASENUM)
				if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {	// Коррекция остатка
					qx--;
					rx += v.coef[n - 1];
				}

			u += (e + 1).shift(i + n + 1);			// Сдвиг вправо для корректного выравнивания
			u = u - (v * qx).shift(i);			// Коррекция объекта u
			q.coef[i] = qx;					// Запись значения qx в текущем разряде результата деления q

			if (u.coef[i + n + 1] == 0) {			// Проверка на необходимость коррекции q
				q.coef[i]--;
				u = u + v.shift(i);			// Сдвиг влево
			}

			u -= (e + 1).shift(i + n + 1);			// Коррекция объекта u
			i--;
		}
		while (q.coef[q.len - 1] == 0 && (q.len > 1)) q.len--;

		return q;						// Возврат результата деления
	}
	BigNumber operator % (const BigNumber& bn) {		// Перегрузка оператора (%) деления с остатком (на БЧ)
		BigNumber e;

		if (bn == e) exit(10);
		if (*this == bn) return e;
		if (*this < bn) return *this;
		if (bn.len < 2) {
			BASE x;
			x = bn.coef[0];
			BigNumber u;
			u.coef[0] = *this % x;
			return u;
		}

		int t = len;
		int n = bn.len;
		int m = t - n;
		int i = 0;
		int d = 0;

		BigNumber u(*this);
		BigNumber v(bn);

		DBASE qx = 0;
		DBASE rx = 0;

		d = BASENUM / (v.coef[n - 1] + 1);
		u = u * d;
		v = v * d;
		i = m;

		while (i >= 0) {
			qx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) / v.coef[n - 1];
			rx = (((DBASE)u.coef[i + n] << BASE_SIZE) + u.coef[i + n - 1]) % v.coef[n - 1];

			if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {
				qx--;
				rx += v.coef[n - 1];
			}
			if (rx < BASENUM)
				if ((qx == BASENUM) || (qx * v.coef[n - 2] > ((rx << BASE_SIZE) + u.coef[i + n - 2]))) {
					qx--;
					rx += v.coef[n - 1];
				}

			u += (e + 1).shift(i + n + 1);
			u = u - (v * qx).shift(i);

			if (u.coef[i + n + 1] == 0) u = u + v.shift(i);

			u -= (e + 1).shift(i + n + 1);
			i--;
		}
		u = u / d;	// Для нормализации результата

		return u;	// Вывод результата
	}

	void cout_10() {								// Вывод десятичного числа	
		BigNumber result = *this;
		BigNumber zero(result.len);
		string s; 								// Пустая строка для записи 10-чного представления числа
		zero.len = result.len;

		while (result != zero) {
			BigNumber tmp;
			tmp = result % 10;						// Вычисление остатков от деления на 10

			s.push_back(tmp.coef[0] + '0');					// Добавление этих остатков к строке

			result = result / 10;						// Деление на 10 для перехода к следующему разряду

			zero.len = result.len;
		}

		reverse(s.begin(), s.end());						// Разворот строки для корректного вывода
		cout << "cout_10 = " << s << endl;
	}
	void cin_10() {							// Ввод десятичного числа
		int j = 0;						// Переменнае для итерации по символам строки
		string s;						// Строка для хранения вводимого числа

		cout << "cin_10: ";
		getline(cin, s);

		int lengthStr = s.length();				// Переменная для хранения длины введенной строки
		BASE tmp = 0;

		BigNumber result;					// Объект для хранения числа

		while (j < lengthStr) {
			if ('0' > s[j] || s[j] > '9') {			// Проверка на дурака
				cout << "error";
				exit(0);
			}

			tmp = s[j] - '0';				// Преобразование символа в число
			result = result * BASE(10);			// Сдвиг влево на 1
			BASE digit = (BASE)tmp;				// Временная переменная, которое представляет текущий символ как цифру
			result = result + digit;			// Добавление новой цифры к числу
			j++;
		}

		result.len_norm();	// Нормализация длины числа
		*this = result;		// Запись введенного числа в текущем объекте
	}
};

void test() {							// Проверка работы основных операторов
	srand(time(0));

	int M = 1000;
	int T = 1000;
	int m;
	int n;

	BigNumber A;
	BigNumber B;
	BigNumber C;
	BigNumber D;

	do {
		cout << "T: " << T << endl;
		cout << "m and n: " << m << " " << n << endl;

		m = rand() % M + 1;
		n = rand() % M + 1;

		BigNumber a(m, 1);
		BigNumber b(n, 1);

		A = a;
		B = b;

		C = A / B;
		D = A % B;
	} while (A == C * B + D && A - D == C * B && D < B && --T);
}

void test_decimal() {			// Проверка работы десятичного ввода
	BigNumber decimalInput;

	decimalInput.cin_10();		// Проверка работы ввода в десятичной форме
	decimalInput.cout_10();		// Проверка работы вывода в десятичной форме

	cout << "print_hex: ";
	decimalInput.print_hex();	// Проверка работы вывода в hex форме

	cout << endl;
}

void test_hex() {			// Проверка работы hex ввода
	BigNumber decimalInput;

	cout << "input_hex: ";
	decimalInput.input_hex();	// Проверка работы ввода в hex форме
	cout << "print_hex: ";
	decimalInput.print_hex();	// Проверка работы вывода в hex форме

	cout << endl;
	decimalInput.cout_10();		// Проверка работы вывода в десятичной форме

	cout << endl;
}


int main() {
	test();

	test_decimal();
	test_hex();

	return 0;
}
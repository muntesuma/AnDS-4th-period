// ссылка на онлайн-компилятор №1	https://www.onlinegdb.com/online_c++_compiler
// ссылка на онлайн-компилятор №2	https://www.programiz.com/cpp-programming/online-compiler/

#include <iostream> 
using namespace std;

class Node {
    // private по умолчанию:	

    friend class BinTree;						// друж класс Бинарные Деревья

    // данные:
    int key;							// ключ (целое число)
    Node* left;							// левый потомок
    Node* right;							// правый потомок

public:
    // конструкторы:
    Node(int k = 0, Node* l = NULL, Node* r = NULL) {	// конструктор
        key = k;
        left = l;
        right = r;
    }
    ~Node() {						// деструктор
        key = 0;
        left = NULL;
        right = NULL;
    }
};

struct elem {		// структура для обхода в ширину
    // данные:
    Node* p;	// на узел
    elem* next;	// на след эл-т
};

class BinTree {
protected:
    // данные:
    Node* root;						// корневой узел

    // методы:
    Node* copy_node(Node* n, Node* node) {				// копирование дерева
        if (!node) {
            return NULL;
        }
        n = new Node(node->key);
        n->left = copy_node(n->left, node->left);
        n->right = copy_node(n->right, node->right);
        return n;
    }

    void delete_tree(Node* node) {			// удаление дерева
        if (node) {
            delete_tree(node->left);
            delete_tree(node->right);
            delete node;
        }
    }

    void print_tree(int k, Node* node) {			// горизонтальный вывод дерева (вспомогательная функция)
        if (node != NULL) {
            print_tree(k + 5, node->right);
            for (int i = 0; i < k; i++) {
                cout << " ";
            }
            cout << node->key << endl;
            print_tree(k + 5, node->left);
        }

    }

public:
    // конструкторы:
    BinTree() {					// конструктор по умолчанию
        root == NULL;
    }
    BinTree(int len) {				// конструктор с заданным кол-вом эл-ов
        root = NULL;
        for (int i = 0; i < len; i++) {
            int k = rand() % 100;
            this->add(k);
        }
    }
    BinTree(int len, int* arr) {			// конструктор по массиву
        root = NULL;
        for (int i = 0; i < len; i++) {
            this->add(arr[i]);
        }
    }
    BinTree(const BinTree& tree) {			// конструктор копирования
        root = copy_node(root, tree.root);
    }
    ~BinTree() {					// деструктор
        delete_tree(root);
    }

    // перегрузка:
    BinTree& operator=(const BinTree& tree) {		// перегрузка оператора =(присваивания)
        if (this != &tree) {
            delete_tree(root);			// удаление дерева
            root = copy_node(root, tree.root);
        }
        return *this;
    }

    // прочие методы:
    Node* find(int k) {							// поиск узла по ключу
        if (root == NULL) {
            return NULL;
        }
        Node* n = root;
        while (n) {
            if (n->key == k) {
                return n;
            }
            else if (n->key > k && n->left == NULL) {
                return NULL;
            }
            else if (n->key < k && n->right == NULL) {
                return NULL;
            }
            if (n->key > k) {
                n = n->left;
            }
            else {
                n = n->right;
            }
        }
    }

    void add(int k) {								// добавление узла в дерево
        if (root == NULL) {							// если нет корня
            root = new Node(k);						// новый узел - корневой
            return;
        }
        Node* node = root;
        while (node) {								// проход по узлам древа, пока есть потомки
            if (node->key > k && node->left == NULL) {			// если нет левого поддерева
                node->left = new Node(k);				// то новый узел добавляется слева
                return;							// выход из цикла
            }
            else if (node->key <= k && node->right == NULL) {		// аналогочно, если нет правого поддерева
                node->right = new Node(k);
                return;
            }
            if (node->key > k) {						// если добавляемый узел меньше текущего
                node = node->left;					// проход влево
            }
            else {							// иначе 
                node = node->right;					// проход вправо
            }

        }
    }

    Node* del(int k) {									// удаление узла из дерева 
        Node* current = root;								// выбор текущего узла работы
        Node* parent = NULL;
        Node* s;
        Node* r;
        while (current && current->key != k) {					// выбор пути
            parent = current;
            if (current->key > k) {						// если удаляемый узел меньше текущего
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        if (!current) {									// если текущего ключа не существует	
            cout << "Key of the node to be deleted is not found" << endl;		// ключ не найден
            return 0;
        }
        else if (current->left == NULL) { 						// если нет левых потомков
            if (parent && parent->left == current) {				// родитель справа
                parent->left = current->right;
                delete current;							// удаление узла
                return parent->left;						// сдвиг вверх
            }
            else if (parent && parent->right == current) {			// иначе родитель слева
                parent->right = current->right;
                delete current;
                return parent->right;
            }
            else {								// иначе - корень
                s = current->right;						// сдвиг корня
                root = s;
                delete current;
                return root;
            }
        }
        else if (current->right == NULL) { 						// иначе если нет правых потомков, то условия выполняются аналогично
            if (parent && parent->left == current) {
                parent->left = current->left;
                delete current;
                return parent->left;
            }
            else if (parent && parent->right == current) {
                parent->right = current->left;
                delete current;
                return parent->right;
            }
            else {
                s = current->left;
                root = s;
                delete current;
                return root;
            }
        }
        s = current->right; 								// проход в право
        if (s->left == NULL) {							// если нет левых потомков, то замена найдена
            current->key = s->key;						// перенос ключа
            current->right = s->right;						// сдвиг вверх
            delete s;
            return current;
        }
        while (s->left != NULL) {							// проход в крайний левый узел
            r = s;									// замена родителя на потомка
            s = s->left;								// выбор левого узла
        }
        current->key = s->key;
        r->left = s->right;								// замена удаляемого узла выбранным
        delete s;									// очистка памяти
        return current;
    }

    int max() {				// поиск макс-го эл-та
        Node* curr = root;
        while (curr->right) {
            curr = curr->right;
        }
        if (curr) {
            return curr->key;	// возврат значения ключа текущего узла
        }
        return -1;
    }
    int min() {				// поиск мин-го эл-та (аналогичен поиску макс эл-та из-за оссобенностей построения упорядоченного двоичного древа)
        Node* curr = root;
        while (curr->left) {
            curr = curr->left;
        }
        if (curr) {
            return curr->key;
        }
        return -1;
    }

    elem* BFS() {							// обход по уровням (в ширину)
        elem* h = new elem;
        h->p = root;
        h->next = NULL;
        elem* runner = h;
        while (h != NULL) {					// пока список не пуст
            if (h->p->left != NULL) {			// если у текущего узла есть левый потомок 
                elem* left_child = new elem;
                left_child->p = h->p->left;	// учет потомка
                left_child->next = NULL;		// следующий - пустой
                runner->next = left_child;
                runner = runner->next;
            }
            if (h->p->right != NULL) {			// если есть правый потомок (аналогично)
                elem* right_child = new elem;
                right_child->p = h->p->right;
                right_child->next = NULL;
                runner->next = right_child;
                runner = runner->next;
            }
            cout << h->p->key << " ";			// вывод 1 эл-та
            h = h->next;					// переход на след эл-т
        }
        cout << endl;
        return 0;
    }

    void LRN(Node* r = NULL) {		// обход в глубину (ЛПК)	
        if (r == NULL) {
            r = root;
        }
        if (r->left) {
            LRN(r->left);
        }
        if (r->right) {
            LRN(r->right);
        }
        cout << r->key << " ";
    }

    void print() {			// горизонтальный вывод дерева
        print_tree(1, root);
        cout << endl;
    }
};

void test() {
    cout << "1)\n";
    BinTree first(10);				// задание 1ого дерева 10 эл-ами
    first.print();					// вывод
    cout << endl;
    cout << "first min = " << first.min() << endl;	// поиск мин эл-та
    cout << "first max = " << first.max() << endl;	// поиск макс эл-та
    cout << "\n\n";

    cout << "2)\n";
    BinTree second(first);			// задание 2ого дерева 1ым
    second.print();
    cout << endl;
    cout << "try to LRN second = ";
    second.LRN();				// обход в глубину (ЛПК)
    cout << endl;
    cout << "try to BFS second = ";
    second.BFS();				// обход в ширину
    cout << endl;
    cout << "\n\n";

    cout << "3)\n";
    BinTree third(5);	// задание 3его дерева 5 эл-ами
    third.print();
    cout << endl;
    third = first;		// приравнивание 3его древа к 1ому
    cout << "3=1)\n";
    third.print();		// повторный вывод
    cout << "\n\n";

    cout << "4)\n";
    int arrBT[] = { 23, 3, 231, 1, 65, 2, 12, 6 };			// задание массива для 4ого древа
    BinTree fourth(10, arrBT);					// задание 4ого древа
    fourth.print();
    cout << endl;
    cout << "try to find 65 = " << fourth.find(65) << endl;		// поиск существующего эл-та
    cout << "try to find 4253 = " << fourth.find(4253) << endl;	// поиск несуществующего эл-та
    cout << endl;
    cout << "enter the key to delete: ";				// ввод удаляемого узла
    int keydel;
    cin >> keydel;
    fourth.del(keydel);
    cout << "5)\n";							// удаление узла
    fourth.print();							// повторный вывод
}

int main() {
    test();
    return 0;
}
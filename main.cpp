#include <string>
#include <cstddef>
#include <exception>
#include <iostream>
#include <cstdlib>

template <typename T>
class Container {
private:
	struct Elem {
		T data;
		Elem* prev, * next;
	};
	std::size_t m_size;
	Elem* m_front;
	Elem* m_back;
public:
	Container() :m_size(0), m_front(nullptr), m_back(nullptr) {};
	Container(const Container& other) {
		Elem* temp = other.m_front;

		while (temp != other.m_back) {
			push_back(temp->data);
			temp = temp->next;
		}
		push_back(temp->data);
		temp = nullptr;
	}
	~Container() { clear(); };

	void push_front(const T& data) {
		if (empty()) {
			m_front = m_back = new Elem;

			m_front->prev = nullptr;
			m_front->next = nullptr;
			m_front->data = data;
		}
		else {
			Elem* temp = new Elem;

			temp->data = data;
			temp->next = m_front;
			temp->prev = nullptr;

			m_front->prev = temp;
			m_front = temp;
		}
		++m_size;
	}
	void push_back(const T& data) {
		if (empty()) {
			m_front = m_back = new Elem;

			m_back->prev = nullptr;
			m_back->next = nullptr;
			m_back->data = data;
		}
		else {
			Elem* temp = new Elem;

			temp->data = data;
			temp->next = nullptr;
			temp->prev = m_back;

			m_back->next = temp;
			m_back = temp;
		}
		++m_size;
	}

	void pop_front() {
		if (empty()) return;
		if (m_size == 1) {
			delete m_front;
			m_front = m_back = nullptr;
		}
		else {
			m_front = m_front->next;
			delete m_front->prev;
			m_front->prev = nullptr;
		}
		--m_size;
	}
	void pop_back() {
		if (empty()) return;
		if (m_size == 1) {
			delete m_back;
			m_front = m_back = nullptr;
		}
		else {
			m_back = m_back->prev;
			delete m_back->next;
			m_back->next = nullptr;
		}
		--m_size;
	}

	const T& front() const {
		if (empty()) throw Exception("Trying to take an element from an empty container");
		return m_front->data;
	}
	const T& back() const {
		if (empty()) throw Exception("Trying to take an element from an empty container");
		return m_back->data;
	}

	std::size_t size() const { return m_size; };
	bool empty() const { return (m_size == 0); };

	Container<T> operator= (const Container& other) {
		clear();
		Elem* temp = other.m_front;

		while (temp != other.m_back) {
			push_back(temp->data);
			temp = temp->next;
		}
		push_back(temp->data);

		temp = nullptr;

		return *this;
	}

	void reverse() {
		Elem* ptr = m_front;

		while (ptr != nullptr) {
			Elem* temp = ptr->next;
			ptr->next = ptr->prev;
			ptr->prev = temp;
			ptr = temp;
		}

		ptr = m_front;

		m_front = m_back;
		m_front->prev = nullptr;
		m_front->next = m_back->next;

		m_back = ptr;
		m_back->prev = ptr->prev;
		m_back->next = nullptr;
	}

	void clear() { while (m_size > 0) pop_back(); };

	class Iterator {
	private:
		Elem* m_ptr;
	public:
		Iterator(Container::Elem* elem) : m_ptr ( elem) {};
		~Iterator() { m_ptr = nullptr; }

		Iterator& operator++ () {
			if (m_ptr == nullptr) throw Exception("Trying to increment an emty iterator");
			if (m_ptr->next == nullptr) throw Exception("The increment of iterator leads tp out of bounds");

			m_ptr = m_ptr->next;
			return *this;
		}

		Iterator& operator-- ()
		{
			if (m_ptr == nullptr) throw Exception("Trying to decrement an empty iterator");
			if (m_ptr->prev == nullptr) throw Exception("The decrement of iterator leads to out of bounds");

			m_ptr = m_ptr->prev;
			return *this;
		}

		T& operator* () { return m_ptr->data; }

		bool operator== (const Iterator& other) const { return (this->m_ptr == other.m_ptr); }
		bool operator!= (const Iterator& other) const { return (this->m_ptr != other.m_ptr); }
	};

	class reverseIterator {
	private:
		Elem* m_ptr;
	public:
		reverseIterator(Container::Elem* elem) : m_ptr(elem) {};
		~reverseIterator() { m_ptr = nullptr; };

		reverseIterator& operator++ () {

			if (m_ptr == nullptr) throw Exception("Trying to increment an emty iterator");
			if (m_ptr->prev == nullptr) throw Exception("The increment of iterator leads tp out of bounds");

			m_ptr = m_ptr->prev;
			return *this;
		}

		reverseIterator& operator-- () {
			if (m_ptr == nullptr) throw Exception("Trying to decrement an emty iterator");
			if (m_ptr->next == nullptr) throw Exception("The decrement of iterator leads to out od bounds");
		}

		T& operator* () { return m_ptr->data; }

		bool operator== (const reverseIterator& other) const { return (this->m_ptr == other.m_ptr); }
		bool operator!= (const reverseIterator& other) const { return (this->m_ptr != other.m_ptr); }
	};

	Iterator begin() { return Iterator(m_front); }
	Iterator end() { return Iterator(m_back); }
	Iterator begin() const { return Iterator(m_front); }
	Iterator end() const { return Iterator(m_back); }

	reverseIterator rbegin() { return reverseIterator(m_back); }
	reverseIterator rend() { return reverseIterator(m_front); }
	reverseIterator rbegin() const { return reverseIterator(m_back); }
	reverseIterator rend() const { return reverseIterator(m_front); }

	class Exception : public std::exception {
	private:
		std::string m_error;
	public:
		Exception(std::string error) : m_error(error) {};
		const char* what() const noexcept { return m_error.c_str(); };
	};
};

class Object {
protected:
	static std::size_t sm_counter;
public:
	Object() { ++sm_counter; };
	virtual ~Object() { --sm_counter; };

	virtual std::string toString() const = 0;
};

std::size_t Object::sm_counter = 0;

class Task : virtual public Object {
protected:
	bool m_executed = false;
public:
	virtual void execute() = 0;
};

class Named : virtual public Object {
protected:
	std::string m_name;
public:
	Named(std::string const& name) : m_name(name) {};
};

class ArithmeticOperation : public Task, public Named {
private:
	double m_leftOperand, m_rightOperand, m_result;
	char m_operation;
	bool m_correctTask = true;
public:
	ArithmeticOperation(std::string const& name, double const leftOperand, char const operation, double const rightOperand)
		: Named(name), m_leftOperand(leftOperand), m_rightOperand(rightOperand), m_operation(operation) {};

	std::string toString() const override {
		if (m_executed)
			return("Task '" + m_name + "': " + std::to_string(m_leftOperand) + " " + std::string(1, m_operation) +
				" " + std::to_string(m_rightOperand) + " = " + std::to_string(m_result));
		else
		{
			if (!m_correctTask)
				return("Incorrect operation: " + std::to_string(m_leftOperand) + " " +
					std::string(1, m_operation) + " " + std::to_string(m_rightOperand));
			else
				return std::string("Calculation is not executed yet.");
		}
	}

	void execute() {
		if (m_executed) return;
		switch (m_operation) {
		case '+':
			m_result = m_leftOperand + m_rightOperand;
			m_executed = true;
			break;

		case '-':
			m_result = m_leftOperand - m_rightOperand;
			m_executed = true;
			break;

		case '*':
			m_result = m_leftOperand * m_rightOperand;
			m_executed = true;
			break;

		case '/':
			if (m_rightOperand == 0)
			{
				m_correctTask = false;
				break;
			}
			m_result = m_leftOperand / m_rightOperand;
			m_executed = true;
			break;

		default:
			m_correctTask = false;
			break;
		}
	}
};

class AddingTaskToList : public Task {
private:
	Task& m_task;
	Container<Task*>& m_container;
public:
	AddingTaskToList(Task& task, Container<Task*>& container)
		: m_task(task), m_container(container) {};
	AddingTaskToList(Task* task, Container<Task*>& container)
		: m_task(*task), m_container(container) {};
	std::string toString() const override {
		if (m_executed)
			return std::string("Task was added in container");
		else
			return std::string("Task is not added in container");
	}
	void execute() override {
		if (m_executed) return;
		m_container.push_back(&m_task);
		m_executed = true;
	}
};

class CountingListObjects : public Task {
private:
	Container<Object*>& m_container;
	std::size_t m_result;
public:
	CountingListObjects(Container<Object*>& container) : m_container(container) {};

	std::string toString() const override {
		if (m_executed)
			return std::string(std::to_string(m_result) + "objects in container");
		else
			return std::string("Counting is not executed");
	}

	void execute() override {
		m_result = m_container.size();
		m_executed = true;
	}
};

class ClearingListObjects : public Task {
private:
	Container<Object*>& m_container;
public:
	ClearingListObjects(Container<Object*> container)
		:m_container(container) {};
	
	std::string toString() {
		if (m_executed)
			return ("Container is cleared");
		else
			return ("Clearing is not executed");
	}

	void execute() {
		m_container.clear();
		m_executed = true;
	}
};

class CountingProgramObjects : public Task {
private:
	std::size_t m_result;
public:
	std::string toString() const override {
		if (m_executed)
			return std::string(std::to_string(m_result) + " objects in program now");
		else
			return std::string("Counting objects in program is not executed");
	}
	void execute() override {
		m_result = sm_counter;
		m_executed = true;
	}
};

int getRandomInt(int min, int max) {
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

double getRandomDouble(double min, double max) {
	static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
	return (rand() * fraction * (max - min + 1) + min);
}

Task * generateArithmeticTask()
{
	switch (getRandomInt(1, 4))
	{
	case 1:
		return new ArithmeticOperation("add", getRandomDouble(-100.0, 100.0), '+', getRandomDouble(-100.0, 100.0));
	case 2:
		return new ArithmeticOperation("sub", getRandomDouble(-100.0, 100.0), '-', getRandomDouble(-100.0, 100.0));
	case 3:
		return new ArithmeticOperation("mul", getRandomDouble(-100.0, 100.0), '*', getRandomDouble(-100.0, 100.0));
	case 4:
		return new ArithmeticOperation("div", getRandomDouble(-100.0, 100.0), '/', getRandomDouble(-100.0, 100.0));
	}
}


int main() {
	srand(time(NULL));
	Container<Task*> tasksList;
	for (std::size_t i = 0; i < 15; ++i)
	{
		Task* task = generateArithmeticTask();
		AddingTaskToList adding(task, tasksList);
		adding.execute();
	}

	CountingProgramObjects objectCounter;
	objectCounter.execute();
	std::cout << objectCounter.toString() << std::endl << std::endl;


	Container<std::string> infoList;
	while (!tasksList.empty())
	{
		Task* task = tasksList.front();
		task->execute();
		infoList.push_back(task->toString());
		delete task;
		tasksList.pop_front();
	}

	Container<std::string>::Iterator iter = infoList.begin();
	for (; iter != infoList.end(); ++iter)
	{
		std::cout << *iter << std::endl;
	}
	std::cout << *iter << std::endl;

	infoList.clear();

	objectCounter.execute();
	std::cout << std::endl << objectCounter.toString() << std::endl;

	return 0;
};
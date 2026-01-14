#include <iostream>
#include <vector>
#include <functional>
#include "../MyStack.hpp"

enum class chunk_policy { constant, linear, square, exponential };

std::vector<std::function<size_t(size_t _idx)>> generate_chunk_size
{
    [](size_t _idx) {return 10; },
    [](size_t _idx) {return 10 * _idx; },
    [](size_t _idx) {return 10 * _idx * _idx; },
    [](size_t _idx) {return 1 << _idx; }
};

template <typename T>
class ChunkStack : public MyStack<T> {
    std::function<size_t(size_t)>* get_chunk_size;
    struct chunk
    {
        T* data;
        chunk* next;
        size_t capacity;
        size_t count;
        size_t idx;
        chunk(size_t _idx, chunk_policy cp) : capacity(generate_chunk_size[static_cast<size_t>(cp)](_idx)), count(0), idx(_idx), next(nullptr)
        {
            data = new T[capacity];
        }
        ~chunk()
        {
            delete[] data;
        }
    };
    chunk* last = nullptr, * current = nullptr;
    chunk_policy cp = chunk_policy::square;


    void copy_from(const ChunkStack<T>& other) {
        if (!other.size()) return;
        std::vector<T> elements;

        chunk* temp = other.last;
        while (temp != nullptr) {
            for (size_t i = 0; i < temp->count; ++i)
                elements.push_back(temp->data[i]);
            temp = temp->next;
        }

        for (auto& v : elements)
            this->push(v);
    }
public:
    ChunkStack(chunk_policy pol = chunk_policy::square) : cp(pol) {
        last = nullptr;
        current = nullptr;
        this->count = 0;
    }

    ChunkStack(const ChunkStack<T>& other) : last(nullptr), current(nullptr), cp(other.cp) {
        this->count = other.count;
        this->copy_from(other);
    }

    ChunkStack(ChunkStack<T>&& other) : last(other.last), current(other.current), cp(other.cp) {
        this->count = other.count;
        
        other.last = nullptr;
        other.current = nullptr;
        other.count = 0;
    }

    ChunkStack<T>& operator=(const ChunkStack<T>& other) {
        if (this != &other) {
            this->~ChunkStack();
            this->cp = other.cp;
            copy_from(other);
        }
        return *this;
    }
    ChunkStack<T>& operator=(ChunkStack<T>&& other) {
        if (this != &other) {
            this->~ChunkStack();
            last = other.last;
            current = other.current;
            this->count = other.count;
            cp = other.cp;

            other.last = nullptr;
            other.current = nullptr;
            other.count = 0;
        }
        return *this;
    }

    ~ChunkStack() {
        while (last != nullptr)
        {
            auto temp = last->next;
            delete last;
            last = temp;
        }
    }
    void push(const T& value) override {
        if (current == nullptr)
        {
            current = new chunk(1, cp);
            current->idx = 1;
            last = current;
        }
        else if (current->count == current->capacity)
        {
            if (last != current)
                current = last;
            else
            {
                last = new chunk(current->idx + 1, cp);
                last->idx = current->idx + 1;
                last->next = current;
                current = last;
            }
        }
        current->data[current->count++] = value;
        this->count++;
    }

    void push(T&& value) override {
        if (current == nullptr)
        {
            current = new chunk(1, cp);
            current->idx = 1;
            last = current;
        }
        else if (current->count == current->capacity)
        {
            if (last != current)
                current = last;
            else
            {
                last = new chunk(current->idx + 1, cp);
                last->idx = current->idx + 1;
                last->next = current;
                current = last;
            }
        }
        current->data[current->count++] = value;
        this->count++;
    }
    /*void pop() override {
        if (this->count == 0)
            throw std::out_of_range("Stack is empty");
        if (current->count == 0)
        {
            if (last != current)
            {
                delete last;
                last = current;
            }
            current = current->next;
        }
        current->count--;
        this->count--;
    }*/

    void pop() override {
        if (this->count == 0)
            throw std::out_of_range("Stack is empty");

        // Удаляем элемент из текущего chunk
        current->count--;
        this->count--;

        // Если chunk пуст - переходим к следующему
        if (current->count == 0) {
            chunk* old_current = current;

            // Ищем следующий непустой chunk
            current = current->next;

            // Если current стал nullptr, но head еще указывает на старый chunk
            if (current == nullptr) {
                // Удаляем старый chunk и обнуляем указатели
                last = nullptr;
                delete old_current;
                current = nullptr;
            }
            else {
                // Удаляем старый chunk из списка
                last = current;
                delete old_current;
            }
        }
    }
    const T& top() const override {
        if (this->count == 0)
            throw std::out_of_range("Stack is empty");
        auto actual = current->count ? current : current->next;
        return actual->data[actual->count - 1];
    }
    T& top() override {
        if (this->count == 0)
            throw std::out_of_range("Stack is empty");
        auto actual = current->count ? current : current->next;
        return actual->data[actual->count - 1];
    }
};
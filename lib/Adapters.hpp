#pragma once
#include <iostream>
#include <variant>

template<class Iterator>
concept BiDirectional = requires(Iterator it) {
  --it;
};

template<class T>
concept STL = requires(T a) {
  a.cbegin();
  a.cend();
};

template<class T>
concept KeyValue = requires(T a) {
  a.cbegin();
  a.cend();
  typename T::key_type;
  typename T::mapped_type;
};

template<class T>
concept Key = requires(T a) {
  a.cbegin();
  a.cend();
  typename T::key_type;
};

template<class T>
concept PairIter = requires(T a) {
  std::get<0>(*a);
  std::get<1>(*a);
};

template<typename ContainerIterator, typename Function>
class TransformIterator {
public:
  TransformIterator(ContainerIterator in_container_iterator, Function in_function) :
    function(in_function), container_iterator(in_container_iterator) {}
  
  void operator--() requires BiDirectional<ContainerIterator> {
    --container_iterator;
  }

  void operator++() {
    ++container_iterator;
  }
  
  auto operator*() const {
    return function(*container_iterator);
  }

  bool operator==(const TransformIterator& other) const {
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const TransformIterator& other) const {
    return container_iterator != other.container_iterator;
  }

private:
  Function function;
  ContainerIterator container_iterator;
};

template<STL Container, typename Function>
class TransformAdapter {
public:
  using ContainerIterator = decltype(Container().cbegin());
  using ContainerType = decltype(std::declval<ContainerIterator>().operator*());

  TransformAdapter() {}

  TransformAdapter(const Container& in_container, Function in_function) :
    function(in_function), container(in_container) {}
  
  TransformIterator<ContainerIterator, Function> begin() const {
    return TransformIterator<ContainerIterator, Function>(container.cbegin(), function);
  }

  TransformIterator<ContainerIterator, Function> cbegin() const {
    return begin();
  }

  TransformIterator<ContainerIterator, Function> end() const {
    return TransformIterator<ContainerIterator, Function>(container.cend(), function);
  }

  TransformIterator<ContainerIterator, Function> cend() const {
    return end();
  }

private:
  Function function;
  const Container& container;
};

template<typename Function>
class TransformFunction {
public:
  TransformFunction(Function in_transforamation) : transformation(in_transforamation) {}
  Function transformation;
};

template<typename Function>
TransformFunction<Function> transform(Function function) {
  return TransformFunction<Function>(function);
}

template<typename Container, typename Function>
TransformAdapter<Container, Function> operator|(const Container& container, TransformFunction<Function> transformation) {
  return TransformAdapter<Container, Function>(container, transformation.transformation);
}

template<typename ContainerIterator, typename Function>
class FilterIterator {
public:
  FilterIterator(ContainerIterator in_container_iterator,
    ContainerIterator in_container_begin_iterator,
    ContainerIterator in_container_end_iterator,
    Function in_function) :
    function(in_function),
    container_iterator(in_container_iterator),
    container_begin_iterator(in_container_begin_iterator),
    container_end_iterator(in_container_end_iterator)
  {}

  void operator--() requires BiDirectional<ContainerIterator> {
    --container_iterator;
    while (container_iterator != container_begin_iterator && !function(*container_iterator)) {
      --container_iterator;
    }
  }

  void operator++() {
    ++container_iterator;
    while (container_iterator != container_end_iterator && !function(*container_iterator)) {
      ++container_iterator;
    }
  }

  auto operator*() const {
    return *container_iterator;
  }

  bool operator==(const FilterIterator& other) const {
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const FilterIterator& other) const {
    return !operator==(other);
  }

private:
  Function function;
  ContainerIterator container_iterator;
  ContainerIterator container_begin_iterator;
  ContainerIterator container_end_iterator;
};

template<STL Container, typename Function>
class FilterAdapter {
public:
  using ContainerIterator = decltype(Container().cbegin());

  FilterAdapter() {}

  FilterAdapter(const Container& in_container, Function in_function) :
    function(in_function), container(in_container) {}

  FilterIterator<ContainerIterator, Function> begin() const {
    auto iterator = container.cbegin();
    while (!function(*iterator) && iterator != container.cend()) {
      ++iterator;
    }
    return FilterIterator<ContainerIterator, Function>(iterator, container.begin(), container.cend(), function);
  }

  FilterIterator<ContainerIterator, Function> cbegin() const {
    return begin();
  }

  FilterIterator<ContainerIterator, Function> end() const {
    return FilterIterator<ContainerIterator, Function>(container.cend(), container.begin(), container.cend(), function);
  }

  FilterIterator<ContainerIterator, Function> cend() const {
    return end();
  }

private:
  Function function;
  const Container& container;
};

template<typename Function>
class FilterFunction {
public:
  FilterFunction(Function in_filtration) : filtration(in_filtration) {}
  Function filtration;
};

template<typename Function>
FilterFunction<Function> filter(Function function) {
  return FilterFunction<Function>(function);
}

template<typename Container, typename Function>
FilterAdapter<Container, Function> operator|(const Container& container, FilterFunction<Function> filtration) {
  return FilterAdapter<Container, Function>(container, filtration.filtration);
}

template<typename ContainerIterator>
class TakeIterator {
public:
  TakeIterator(ContainerIterator in_container_iterator) : container_iterator(in_container_iterator) {}

  void operator--() requires(BiDirectional<ContainerIterator>) {
    --container_iterator;
  }
  void operator++() {
    ++container_iterator;
  }
  auto operator*() const {
    return *container_iterator;
  }

  bool operator==(const TakeIterator& other) const {
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const TakeIterator& other) const {
    return !operator==(other);
  }

private:
  ContainerIterator container_iterator;
};

template<STL Container>
class TakeAdapter {
public:
  using ContainerIterator = decltype(Container().cbegin());

  TakeAdapter() {}

  TakeAdapter(const Container& in_container, const int in_N) : container(in_container), N(in_N) {}

  TakeIterator<ContainerIterator> begin() const {
    return TakeIterator<ContainerIterator>(container.cbegin());
  }

  TakeIterator<ContainerIterator> cbegin() const {
    return begin();
  }

  TakeIterator<ContainerIterator> end() const {
    auto iterator = container.cbegin();
    for (int i = 0; i < N; ++i) {
      ++iterator;
      if (iterator == container.cend()) {
        break;
      }
    }
    return TakeIterator<ContainerIterator>(iterator);
  }

  TakeIterator<ContainerIterator> cend() const {
    return end();
  }

private:
  const Container& container;
  const int N;
};

class TakeN {
public:
  TakeN(int in_N) : N(in_N) {}
  const int N;
};

TakeN take(int N) {
  return TakeN(N);
}

template<typename Container>
TakeAdapter<Container> operator|(const Container& container, TakeN N) {
  return TakeAdapter<Container>(container, N.N);
}

template<typename ContainerIterator>
class DropIterator {
public:
  DropIterator(ContainerIterator in_container_iterator) : container_iterator(in_container_iterator) {}

  void operator--() requires(BiDirectional<ContainerIterator>) {
    --container_iterator;
  }
  void operator++() {
    ++container_iterator;
  }
  auto operator*() const {
    return *container_iterator;
  }

  bool operator==(const DropIterator& other) const {
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const DropIterator& other) const {
    return !operator==(other);
  }

private:
  ContainerIterator container_iterator;
};

template<STL Container>
class DropAdapter {
public:

  using ContainerIterator = decltype(Container().cbegin());

  DropAdapter() {}

  DropAdapter(const Container& in_container, const int in_N) : container(in_container), N(in_N) {}

  DropIterator<ContainerIterator> begin() const {
    auto iterator = container.cbegin();
    for (int i = 0; i < N; ++i) {
      ++iterator;
      if (iterator == container.cend()) {
        break;
      }
    }
    return DropIterator<ContainerIterator>(iterator);
  }

  DropIterator<ContainerIterator> cbegin() const {
    return begin();
  }

  DropIterator<ContainerIterator> end() const {
    return DropIterator<ContainerIterator>(container.cend());
  }

  DropIterator<ContainerIterator> cend() const {
    return end();
  }

private:
  const Container& container;
  const int N;
};

class DropN {
public:
  DropN(int in_N) : N(in_N) {}
  const int N;
};

DropN drop(int N) {
  return DropN(N);
}

template<typename Container>
DropAdapter<Container> operator|(const Container& container, DropN N) {
  return DropAdapter<Container>(container, N.N);
}

template<typename ContainerIterator>
class ReverseIterator {
public:
  ReverseIterator(ContainerIterator in_container_iterator, ContainerIterator in_container_begin_iterator) :
    container_iterator(in_container_iterator),
    container_begin_iterator(in_container_begin_iterator)
  {}

  void operator--() {
    ++container_iterator;
  }

  void operator++() {
    if (container_iterator == container_begin_iterator) {
      current_end_phase = true;
    }
    if (!current_end_phase) {
      --container_iterator;
    }
  }
  auto operator*() const {
    return *container_iterator;
  }

  bool operator==(const ReverseIterator& other) const {
    if (other.container_iterator == container_begin_iterator) {
      return current_end_phase;
    }
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const ReverseIterator& other) const {
    return !operator==(other);
  }
private:
  ContainerIterator container_iterator;
  ContainerIterator container_begin_iterator;
  bool current_end_phase = false;
};

template<STL Container>
requires(BiDirectional<decltype(Container().cend())>)
class ReverseAdapter { 
public:
  using ContainerIterator = decltype(Container().cbegin());

  ReverseAdapter() {}

  ReverseAdapter(const Container& in_container) : container(in_container) {}

  ReverseIterator<ContainerIterator> begin() const {
    auto iterator = container.cend();
    --iterator;
    return ReverseIterator<ContainerIterator>(iterator, container.cbegin());
  }

  ReverseIterator<ContainerIterator> cbegin() const {
    return begin();
  }

  ReverseIterator<ContainerIterator> end() const {
    auto iterator = container.cbegin();
    return ReverseIterator<ContainerIterator>(iterator, container.cbegin());
  }

  ReverseIterator<ContainerIterator> cend() const {
    return end();
  }
private:
  const Container& container;
};

class reverse {};

template<typename Container>
ReverseAdapter<Container> operator|(const Container& container, reverse) {
  return ReverseAdapter<Container>(container);
}

template<typename ContainerIterator>
class KeysIterator {
public:
  KeysIterator(ContainerIterator in_container_iterator) : container_iterator(in_container_iterator) {}

  void operator--() requires(BiDirectional<ContainerIterator>) {
    --container_iterator;
  }

  void operator++() {
    ++container_iterator;
  }

  auto operator*() const requires(PairIter<ContainerIterator>) {
    return std::get<0>(*container_iterator);
  }

  auto operator*() const requires(!PairIter<ContainerIterator>) {
    return *container_iterator;
  }

  bool operator==(const KeysIterator& other) const {
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const KeysIterator& other) const {
    return !operator==(other);
  }

private:
  ContainerIterator container_iterator;
};

template<Key Container>
class KeysAdapter {
public:
  using ContainerIterator = decltype(Container().cbegin());
  using ContainerType = decltype(std::declval<ContainerIterator>().operator*());

  KeysAdapter() {}
  KeysAdapter(const Container& in_container) : container(in_container) {}

  KeysIterator<ContainerIterator> begin() const {
    return KeysIterator<ContainerIterator>(container.cbegin());
  }

  KeysIterator<ContainerIterator> cbegin() const {
    return begin();
  }

  KeysIterator<ContainerIterator> end() const {
    return KeysIterator<ContainerIterator>(container.cend());
  }

  KeysIterator<ContainerIterator> cend() const {
    return end();
  }

private:
  const Container& container;
};

class keys {};

template<typename Container>
KeysAdapter<Container> operator|(const Container& container, keys) {
  return KeysAdapter<Container>(container);
}

template<typename ContainerIterator>
class ValuesIterator {
public:
  ValuesIterator(ContainerIterator in_container_iterator) : container_iterator(in_container_iterator) {}

  void operator--() requires(BiDirectional<ContainerIterator>) {
    --container_iterator;
  }

  void operator++() {
    ++container_iterator;
  }

  auto operator*() const {
    return std::get<1>(*container_iterator);
  }

  bool operator==(const ValuesIterator& other) const {
    return container_iterator == other.container_iterator;
  }

  bool operator!=(const ValuesIterator& other) const {
    return !operator==(other);
  }

private:
  ContainerIterator container_iterator;
};

template<KeyValue Container>
class ValuesAdapter {
public:
  using ContainerIterator = decltype(Container().cbegin());
  using ContainerType = decltype(std::declval<ContainerIterator>().operator*());

  ValuesAdapter() {}
  ValuesAdapter(const Container& in_container) : container(in_container) {}

  ValuesIterator<ContainerIterator> begin() const {
    return ValuesIterator<ContainerIterator>(container.cbegin());
  }

  ValuesIterator<ContainerIterator> cbegin() const {
    return begin();
  }

  ValuesIterator<ContainerIterator> end() const {
    return ValuesIterator<ContainerIterator>(container.cend());
  }

  ValuesIterator<ContainerIterator> cend() const {
    return end();
  }

private:
  const Container& container;
};

class values {};

template<typename Container>
ValuesAdapter<Container> operator|(const Container& container, values) {
  return ValuesAdapter<Container>(container);
}

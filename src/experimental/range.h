#ifndef LSS_EXPERIMENTAL_COLLECTION_H_
#define LSS_EXPERIMENTAL_COLLECTION_H_

template<class Iterator>
class Range {
 public:
  Range(const Range& range) = default;
  Range(Iterator begin, Iterator end) : begin_(begin), end_(end) {}

  Iterator begin() const { return begin_; }
  Iterator end() const { return end_; }

 private:
  Iterator begin_;
  Iterator end_;
};

#endif  // LSS_EXPERIMENTAL_COLLECTION_H_

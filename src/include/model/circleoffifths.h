//
// Created by Osbert Nephide Ngok on 17/12/2024.
//

#ifndef CIRCLE_OF_FIFTHS_H
#define CIRCLE_OF_FIFTHS_H

namespace chordnovarw::model {

  /**
   * \brief the position of the pitch class in Circle of Fifths.
   *
   * C is 0. F is -1. G is 1.
   */
  class Chroma {
  public:
    explicit Chroma(int chroma);
    [[nodiscard]] int get_chroma() const;

    bool operator<(const Chroma &chroma) const;
    bool operator>(const Chroma &chroma) const;
    bool operator==(const Chroma &chroma) const;

  private:
    int _chroma;

  };
}

#endif //CIRCLE_OF_FIFTHS_H

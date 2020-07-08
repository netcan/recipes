/*************************************************************************
    > File Name: Frame.cpp
    > Author: Netcan
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2019-10-29 23:01
************************************************************************/
#include <iostream>
#include <vector>

struct IPicture {
    virtual void display(std::ostream& o, size_t row, size_t len) = 0;
    virtual size_t width() = 0;
    virtual size_t height() = 0;
    virtual ~IPicture() = default;
};

static void pad(std::ostream& o, size_t cnt, char c = ' ') {
    while (cnt-- > 0) o << c;
}

struct StringPic: IPicture {
    StringPic(std::vector<std::string> strs) :
        m_strs(std::move(strs)), m_height(m_strs.size()), m_width(0) {
            for (auto& s: m_strs) { m_width = std::max(m_width, s.length()); }
        }
    size_t width()  override { return m_width;  }
    size_t height() override { return m_height; }
    void display(std::ostream& o, size_t row, size_t len) override {
        size_t l = 0;
        if (row < m_height) {
            assert(width() <= len);
            o << m_strs[row];
            l = m_strs[row].length();
        }
        pad(o, len - l);
    }
private:
    std::vector<std::string> m_strs;
    size_t m_height;
    size_t m_width;
};

struct FramePic: IPicture {
    FramePic(const std::shared_ptr<IPicture> &pic): m_pic(pic) {};
    size_t width()  override { return m_pic->width() + 2;  }
    size_t height() override { return m_pic->height() + 2; }
    void display(std::ostream& o, size_t row, size_t len) override {
        size_t l = 0;
        if (row < height()) {
            assert(width() <= len);
            if (row == 0 || row == height() - 1) {
                o << '+'; pad(o, m_pic->width(), '-'); o << '+';
            } else {
                o << '|'; m_pic->display(o, row - 1, m_pic->width()); o << '|';
            }
            l = width();
        }
        pad(o, len - l);
    }
private:
    std::shared_ptr<IPicture> m_pic;
};

struct VCatPic: IPicture {
    VCatPic(const std::shared_ptr<IPicture> &top, const std::shared_ptr<IPicture> &bottom):
        m_top(top), m_bottom(bottom) {};
    size_t width()  override { return std::max(m_top->width(), m_bottom->width()); }
    size_t height() override { return m_top->height() + m_bottom->height(); }
    void display(std::ostream& o, size_t row, size_t len) override {
        size_t l = 0;
        if (row < height()) {
            assert(width() <= len);
            row < m_top->height() ?
                  m_top->display(o, row, len) :
                  m_bottom->display(o, row - m_top->height(), len);
            l = width();
        }
        pad(o, len - l);
    }

private:
    std::shared_ptr<IPicture> m_top, m_bottom;
};

struct HCatPic: IPicture {
    HCatPic(const std::shared_ptr<IPicture> &left, const std::shared_ptr<IPicture> &right):
        m_left(left), m_right(right) {}
    size_t width()  override { return m_left->width() + m_right->width(); }
    size_t height() override { return std::max(m_left->height(), m_right->height()); }
    void display(std::ostream& o, size_t row, size_t len) override {
        size_t l = 0;
        if (row < height()) {
            assert(width() <= len);
            m_left->display(o, row, m_left->width());
            m_right->display(o, row, m_right->width());
            l = width();
        }
        pad(o, len - l);
    }
private:
    std::shared_ptr<IPicture> m_left, m_right;
};

std::shared_ptr<IPicture> mkSP(const std::vector<std::string>& strs) {
    return std::make_shared<StringPic>(strs);
}

std::shared_ptr<IPicture> operator+(const std::vector<std::string>& strs) {
    return std::make_shared<StringPic>(strs);
}

std::shared_ptr<IPicture> operator&(const std::shared_ptr<IPicture> &top, const std::shared_ptr<IPicture> &bottom) {
    return std::make_shared<VCatPic>(top, bottom);
}

std::shared_ptr<IPicture> operator|(const std::shared_ptr<IPicture> &left, const std::shared_ptr<IPicture> &right) {
    return std::make_shared<HCatPic>(left, right);
}

std::shared_ptr<IPicture> operator~(const std::shared_ptr<IPicture> &pic) {
    return std::make_shared<FramePic>(pic);
}

std::ostream& operator<<(std::ostream& o, const std::shared_ptr<IPicture> &pic) {
    for (size_t h = 0; h < pic->height(); ++h) {
        pic->display(o, h, pic->width());
        o << std::endl;
    }
    return o;
}

int main(int argc, char** argv) {
    auto pic = mkSP({"hello", "netcan", "excellent"});

    std::cout << ~(~~((~(pic | ((~pic) & pic))) & (~pic)) | ~(~pic & pic));

    return 0;
}

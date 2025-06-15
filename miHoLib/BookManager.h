#ifndef BOOKMANAGER_H
#define BOOKMANAGER_H

#include "Book.h"
#include <vector>
#include <string>

class BookManager {
private:
    BookManager();
    BookManager(const BookManager&) = delete;
    BookManager& operator=(const BookManager&) = delete;


    std::vector<Book*> books;
    const std::string bookFilepath = "../../res/data/books.dat";
    
    void loadBooks();


public:
    ~BookManager();
    static BookManager& getInstance();
    void saveBooks();
    // 增删改
    void addBook(const std::string& title, 
                const std::string& author, 
                const std::string& isbn, 
                const std::string& category);
    void removeBook(const std::string& isbn);
    void updateBook(const std::string& isbn, 
                   const std::string& title, 
                   const std::string& author, 
                   const std::string& category);

    // 更新图书库存
    void updateBookCopies(const std::string& isbn, int totalCopies);
    
    // 批量更新图书库存
    void batchUpdateBookCopies(const std::vector<std::string>& isbns, int totalCopies);

    
//===============查找书本(getter)=============================
    // 获取所有图书
    const std::vector<Book*>& getAllBooks() const;
    
    // 按分类获取图书
    std::vector<Book*> getBooksByCategory(const std::string& category) const;
    
    // 搜索图书（按书名或作者）
    std::vector<Book*> searchBooks(const std::string& keyword) const;
    
    // 根据ISBN查找图书
    Book* findBookByISBN(const std::string& isbn) const;

    // 使用通配符搜索书名
    std::vector<Book*> searchBooksByTitleWithWildcard(const std::string& pattern) const;

    // 按作者和出版社联合查询
    std::vector<Book*> searchBooksByAuthorAndPublisher(const std::string& author, const std::string& publisher) const;

    // 获取指定ISBN列表的图书
    std::vector<Book*> getBooksByISBNList(const std::vector<std::string>& isbnList) const;
};

#endif // BOOKMANAGER_H

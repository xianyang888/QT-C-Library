#include "BookManager.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <QDebug>
#include <regex>


BookManager::BookManager() {
    loadBooks();
}

BookManager::~BookManager() {
    for(auto book : books) {
        delete book;
    }
}

BookManager& BookManager::getInstance() {
    static BookManager instance;
    return instance;
}


void BookManager::loadBooks() {
    // 清空当前图书列表
    for (auto book : books) {
        delete book;
    }
    books.clear();
    
    // 尝试打开文件
    std::ifstream bookFile(bookFilepath);
    if (!bookFile.is_open()) {
        // 如果文件不存在，创建空文件
        if (!std::filesystem::exists(bookFilepath)) {
            // 确保目录存在
            std::filesystem::path dirPath = std::filesystem::path(bookFilepath).parent_path();
            std::filesystem::create_directories(dirPath);
            
            // 创建空文件
            saveBooks();
        } else {
            qDebug() << u8"图书数据文件打开失败";
        }
        return;
    }
    
    // 读取图书数据
    std::string line;
    while (std::getline(bookFile, line)) {
        Book* book = Book::deserialize(line);
        if (book) {
            books.push_back(book);
        }
    }
    bookFile.close();
}

void BookManager::saveBooks() {
    // 确保目录存在
    std::filesystem::path dirPath = std::filesystem::path(bookFilepath).parent_path();
    std::filesystem::create_directories(dirPath);
    
    // 打开文件
    std::ofstream bookFile(bookFilepath);
    if (!bookFile.is_open()) {
        throw std::runtime_error(u8"图书数据文件保存失败");
    }
    
    // 写入图书数据
    for (const auto& book : books) {
        bookFile << book->serialize() << "\n";
    }
    bookFile.close();
}

void BookManager::addBook(const std::string& title, 
                         const std::string& author, 
                         const std::string& isbn, 
                         const std::string& category) {
    // 检查ISBN是否已存在() KEY:
    for (const auto& book : books) {
        if (book->getISBN() == isbn) {
            throw std::runtime_error(u8"ISBN已存");
        }
    }
    
    // 创建新图书
    Book* newBook = new Book(title, author, isbn, category);
    books.push_back(newBook);
    
    // 保存图书数据
    saveBooks();
}

void BookManager::removeBook(const std::string& isbn) {
    for (auto it = books.begin(); it != books.end(); ++it) {
        if ((*it)->getISBN() == isbn) {
            delete *it;
            books.erase(it);
            saveBooks();
            return;
        }
    }
    throw std::runtime_error(u8"图书不存在");
}

void BookManager::updateBook(const std::string& isbn, 
                            const std::string& title, 
                            const std::string& author, 
                            const std::string& category) {
    for (auto& book : books) {
        if (book->getISBN() == isbn) {
            book->setTitle(title);
            book->setAuthor(author);
            book->setCategory(category);
            saveBooks();
            return;
        }
    }
    throw std::runtime_error(u8"图书不存在");
}

void BookManager::updateBookCopies(const std::string& isbn, int totalCopies) {
    for (auto& book : books) {
        if (book->getISBN() == isbn) {
            book->setTotalCopies(totalCopies);
            saveBooks();
            return;
        }
    }
    throw std::runtime_error(u8"图书不存在");
}

void BookManager::batchUpdateBookCopies(const std::vector<std::string>& isbns, int totalCopies) {
    bool anyUpdated = false;
    
    for (const auto& isbn : isbns) {
        for (auto& book : books) {
            if (book->getISBN() == isbn) {
                book->setTotalCopies(totalCopies);
                anyUpdated = true;
                break;
            }
        }
    }
    
    if (anyUpdated) {
        saveBooks();
    }
}

//==========getter===============================
const std::vector<Book*>& BookManager::getAllBooks() const {
    return books;
}

std::vector<Book*> BookManager::getBooksByCategory(const std::string& category) const {
    if (category.empty() || category == u8"所有分类") {
        return books;
    }
    
    std::vector<Book*> result;
    for (const auto& book : books) {
        if (book->getCategory() == category) {
            result.push_back(book);
        }
    }
    return result;
}

std::vector<Book*> BookManager::searchBooks(const std::string& keyword) const {
    if (keyword.empty()) {
        return books;
    }
    
    std::vector<Book*> result;
    for (const auto& book : books) {
        // 搜索书名或作者
        if (book->getTitle().find(keyword) != std::string::npos ||
            book->getAuthor().find(keyword) != std::string::npos) {
            result.push_back(book);
        }
    }
    return result;
}

Book* BookManager::findBookByISBN(const std::string& isbn) const {
    for (const auto& book : books) {
        if (book->getISBN() == isbn) {
            return book;
        }
    }
    return nullptr;
}

std::vector<Book*> BookManager::searchBooksByTitleWithWildcard(const std::string& pattern) const {
    if (pattern.empty()) {
        return books;
    }
    
    std::vector<Book*> result;
    
    // 将通配符模式转换为正则表达式
    std::string regexPattern = pattern;
    // 替换*为.*（任意字符出现0次或多次）
    size_t pos = 0;
    while ((pos = regexPattern.find('*', pos)) != std::string::npos) {
        regexPattern.replace(pos, 1, ".*");
        pos += 2;
    }
    
    // 替换?为.（任意单个字符）
    pos = 0;
    while ((pos = regexPattern.find('?', pos)) != std::string::npos) {
        regexPattern.replace(pos, 1, ".");
        pos += 1;
    }
    
    try {
        std::regex re(regexPattern, std::regex_constants::icase); // 不区分大小写
        
        for (const auto& book : books) {
            if (std::regex_search(book->getTitle(), re)) {
                result.push_back(book);
            }
        }
    } catch (const std::regex_error& e) {
        // 如果正则表达式无效，退回到简单的子串搜索
        for (const auto& book : books) {
            if (book->getTitle().find(pattern) != std::string::npos) {
                result.push_back(book);
            }
        }
    }
    
    return result;
}

std::vector<Book*> BookManager::searchBooksByAuthorAndPublisher(const std::string& author, const std::string& publisher) const {
    std::vector<Book*> result;
    
    for (const auto& book : books) {
        bool authorMatch = author.empty() || book->getAuthor().find(author) != std::string::npos;
        bool publisherMatch = publisher.empty() || book->getPublisher().find(publisher) != std::string::npos;
        
        if (authorMatch && publisherMatch) {
            result.push_back(book);
        }
    }
    
    return result;
}

std::vector<Book*> BookManager::getBooksByISBNList(const std::vector<std::string>& isbnList) const {
    std::vector<Book*> result;
    
    for (const auto& isbn : isbnList) {
        Book* book = findBookByISBN(isbn);
        if (book) {
            result.push_back(book);
        }
    }
    
    return result;
}

#ifndef PRODUCT_H
#define PRODUCT_H

#include <Arduino.h> // 아두이노 String 클래스 사용을 위해 포함

class Product {
private:
    int productID;
    int categoryID;
public:
    Product(): productID(0), categoryID(0) {}
    Product(int pID, int cID)
        : productID(pID),categoryID(cID) {}
    int getProductID() { return productID; }
    int getCategoryID() { return categoryID; }
    void displayProductInfo() {
        Serial.println("Product ID: " + String(productID));
        Serial.println("Category ID: " + String(categoryID));
    }
};

#endif
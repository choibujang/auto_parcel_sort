#ifndef PRODUCT_H
#define PRODUCT_H

class Product {
private:
    int productID;
    unsigned long categoryID;
public:
    Product(): productID(0), categoryID(0) {}
    Product(int pID, unsigned long cID)
        : productID(pID),categoryID(cID) {}
    int getProductID() { return productID; }
    unsigned long getCategoryID() { return categoryID; }
    void displayProductInfo() {
        Serial.println("Product ID: " + String(productID));
        Serial.println("Category ID: " + String(categoryID));
    }
};

#endif
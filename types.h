#ifndef TYPES_H
#define TYPES_H

#define MAXDESC 51
#define MAXEAN 14
#define MAXPRODUCTS 10000
#define MAXIVA 26
#define MAXLINE 65535

#define EINVALID_EAN    "invalid ean"
#define EINVALID_IVA    "invalid iva"
#define EINVALID_PRICE  "invalid price"
#define EINVALID_QTY    "invalid quantity"
#define EINVALID_DESC   "invalid description"
#define EPRODUCT_IN_USE "product in use"
#define EINVALID_PROD   "invalid product"
#define ENO_STOCK       "no stock"
#define ENO_PRODUCT     "no such product"
#define ENO_INVOICE     "no such invoice"
#define ENO_CLIENT      "no such client"
#define EINVALID_NAME   "invalid name"
#define EINVALID_NIF    "no such nif"
#define ENO_MEMORY      "No memory."

typedef struct {
    char description[MAXDESC];
    char ean[MAXEAN];
    double price;
    char iva;
    int stock;
    int sold;
    int number;
} Product;

typedef struct {
    char ean[MAXEAN];
    int product_index;
    int quantity;
} CartItem;

typedef struct {
    int number;
    int nif;
    char *client_name;
    double value;
    int num_items;
} Invoice;

typedef struct {
    Product products[MAXPRODUCTS];
    int total_products;

    int product_indexes_by_ean[MAXPRODUCTS]; 
    
    CartItem *cart;
    int cart_size;
    
    Invoice *invoices;
    int num_invoices;
    int next_invoice_number;
    int max_invoices;
    
    int iva_taxes[MAXIVA];
} System;

#endif
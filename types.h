/**
 * Definition of the main data structures for the billing system.
 * @file types.h
 * @author ist1117729 (Duarte Veríssimo)
 */

#ifndef TYPES_H
#define TYPES_H

#define MAXDESC 51      /**< max. len. of product description */
#define MAXEAN 14       /**< max. len. of ean code */
#define MAXPRODUCTS 10000       /**< max. number of products */
#define MAXIVA 26       /**< max. number of iva categories */
#define MAXLINE 65535       /**< max. len. of input line */

#define NIF_LENGTH 9        /**< exact length of a nif */
#define DEFAULT_NIF 999999999       /**< default client nif */
#define DEFAULT_CLIENT_NAME "Cliente final"         /**< default client name */

#define EINVALID_EAN "invalid ean"      /**< invalid ean error */
#define EINVALID_IVA "invalid iva"      /**< invalid iva error */
#define EINVALID_PRICE "invalid price"      /**< invalid price error */
#define EINVALID_QTY "invalid quantity"         /**< invalid quantity error */
#define EINVALID_DESC "invalid description"         /**< invalid desc error */
#define EPRODUCT_IN_USE "product in use"        /**< product in use error */
#define EINVALID_PROD "invalid product"         /**< invalid product error */
#define ENO_STOCK "no stock"        /**< no stock error */
#define ENO_PRODUCT "no such product"       /**< non existing product */
#define ENO_INVOICE "no such invoice"       /**< non existing invoice */
#define ENO_CLIENT "no such client"         /**< non existing client */
#define EINVALID_NAME "invalid name"        /**< invalid name error */
#define EINVALID_NIF "no such nif"      /**< non existing nif */
#define ENO_MEMORY "No memory."         /**< memory exhausted */

/** Information about a single product in the system */
typedef struct {
    char description[MAXDESC];      /**< product description */
    char ean[MAXEAN];       /**< EAN-8 or EAN-13 barcode */
    double price;       /**< product price */
    char iva;       /**< IVA class (uppercase letter) */
    int stock;      /**< available quantity in stock */
    int sold;       /**< total quantity of items sold */
} Product;

/** Information about an item currently in the shopping cart */
typedef struct {
    char ean[MAXEAN];       /**< EAN-8 or EAN-13 barcode of the product */
    int product_index;      /**< index of the product in the global array */
    int quantity;       /**< quantity of the product in the cart */
} CartItem;

/** Summary of a invoice */
typedef struct {
    int number;         /**< sequential invoice number */
    int nif;        /**< client's NIF (9 digits) */
    char *client_name;      /**< name of the client */
    double value;       /**< total billed value */
    int num_items;      /**< total number of items purchased */
} Invoice;

/** Main system state containing all products, the shopping cart, and invoices */
typedef struct {
    Product products[MAXPRODUCTS];       /**< array of all products */
    int total_products;      /**< count of products */
    int product_indexes_by_ean[MAXPRODUCTS];        /**< sorted EAN indexes */
    
    CartItem *cart;         /**< dyn. array for cart */
    int cart_size;       /**< distinct items in cart */
    
    Invoice *invoices;       /**< dyn. array of invoices */
    int num_invoices;       /**< current invoices count */
    int next_invoice_number;        /**< sequential next number */
    int max_invoices;       /**< allocated capacity */

    double total_revenue;        /**< total billed value */
    int total_items_sold;       /**< total items purchased */
    
    int iva_taxes[MAXIVA];      /**< array of IVA percentages mapped by letter */
} System;

#endif
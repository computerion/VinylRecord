//
// Created by James Whitfield on 3/18/14.
// Copyright (c) 2014 NEiLAB, Inc. All rights reserved.
//
//TODO: Extract from category and Incorpate in ActiveRecord class.

#import <Foundation/Foundation.h>
#import "ActiveRecord.h"
#define add_search_on(aField) \
    [self performSelector: @selector(addSearchOn:) withObject: @ ""#aField ""];

#define indexes_do(indices) \
    + (void)initializeIndices { \
        [super initializeIndices]; \
        indices \
    }

#define mapping_do(mapping) \
    (void) initializeMapping { \
        return; \
    }

#define belonging_to_imp(class, getter, dependency) \
    + (ARDependency)_ar_registerBelongsTo ## class { \
        return dependency; \
    } \
    -(class *)getter { \
        NSString *class_name = @ ""#class ""; \
        return [self performSelector : @selector(belongsTo:) withObject : class_name]; \
    } \
    -(void)set ## class : (ActiveRecord *)aRecord { \
        NSString *aClassName = @ ""#class ""; \
        objc_msgSend(self, sel_getUid("setRecord:belongsTo:"), aRecord, aClassName); \
    }

#define belonging_to_dec(class, getter, dependency) \
    -(class *)getter; \
    -(void)set ## class : (ActiveRecord *)aRecord;


#define has_none_through_dec has_many_through_imp
#define has_none_through_imp(relative_class, relationship, accessor, dependency) \
    + (ARDependency)_ar_registerHasManyThrough ## relative_class ## _ar_ ## relationship { \
        return dependency; \
    } \
    -(ARLazyFetcher *)accessor \
    { \
        return nil; \
    } \
    -(void)add ## relative_class : (ActiveRecord *)aRecord { \
    } \
    -(void)remove ## relative_class : (ActiveRecord *)aRecord { \
    }




#define AR_DECIMAL_NUM(number) [NSDecimalNumber numberWithDouble: number]
#define AR_DECIMAL_STR(number_string) [NSDecimalNumber decimalNumberWithString: number_string]
#define AR_INTEGER_NUM(number) [NSNumber numberWithInt: number]
#define AR_INTEGER_STR(number_string) [number_string intValue]

#define attr_accessor_dec(ar_type,propertyName) field_##ar_type##_dec(propertyName);
#define attr_accessor_imp(ar_type,propertyName) field_##ar_type##_imp(propertyName);


/* This marco's are helpers to generate methods to allow assigning primitive types */


#define property_as_dec(PrimitiveType,PersistentClass,PropertyName) 1

#define field_key_dec field_integer_dec
#define field_key_imp field_integer_imp


#define field_string_dec( property_name ) \
    @property ( nonatomic,copy ) NSString * property_name;
#define field_string_imp( property_name ) \
    @dynamic property_name ;

#define field_date_dec( property_name ) \
    @property ( nonatomic, strong) NSDate * property_name;
#define field_date_imp( property_name ) \
    @dynamic property_name;


#define field_boolean_dec( property_name ) \
    @property ( nonatomic,strong ) NSNumber * property_name; \
    @property ( nonatomic,assign ) NSInteger is_##property_name ; \
    @property ( nonatomic,assign ) NSInteger has_##property_name ;

#define field_boolean_imp( property_name ) \
    @dynamic property_name ;   \
    - (BOOL) is_##property_name { \
        NSInteger value =  [ [ self property_name ] boolValue ]; \
        return value; \
    }  \
    - (BOOL) has_##property_name { \
        NSInteger value =  [ [ self property_name ] boolValue ]; \
        return value; \
    }  \
    - (void) setIs_##property_name: (NSInteger ) value  { \
        NSString *propertyName =  @"" #property_name ; \
        NSString *propertySetter =  [propertyName asSetterMethodName]; ; \
        objc_msgSend(self,sel_getUid([propertySetter UTF8String]), [NSNumber numberWithInt: value ]) ; \
    } \
    - (void) setHas_##property_name: (NSInteger ) value  { \
        NSString *propertyName =  @"" #property_name ; \
        NSString *propertySetter =  [propertyName asSetterMethodName]; ; \
        objc_msgSend(self,sel_getUid([propertySetter UTF8String]), [NSNumber numberWithInt: value ]) ; \
    }


#define field_integer_dec( property_name ) \
    @property ( nonatomic,strong ) NSNumber * property_name; \
    @property ( nonatomic,assign ) NSInteger int_##property_name ;

#define field_integer_imp( property_name ) \
    @dynamic property_name ;   \
    - (NSInteger) int_##property_name { \
        NSInteger value =  [ [ self property_name ] integerValue ]; \
        return value; \
    }  \
    - (void) setInt_##property_name: (NSInteger ) value  { \
        NSString *propertyName =  @"" #property_name ; \
        NSString *propertySetter =  [propertyName asSetterMethodName] ; \
        objc_msgSend(self,sel_getUid([propertySetter UTF8String]), [NSNumber numberWithInt: value ]) ; \
    }


#define field_decimal_dec( property_name ) \
    @property ( nonatomic,strong ) NSDecimalNumber * property_name; \
    @property ( nonatomic ) double double_##property_name ;

#define field_decimal_imp( property_name ) \
    @dynamic property_name ;   \
    - (double) double_##property_name { \
        NSInteger value =  [ [ self property_name ] integerValue ]; \
        return value; \
    }  \
    - (void) setDouble_##property_name: (double) value  { \
        NSString *propertyName =  @"" #property_name ; \
        NSString *propertySetter =  [propertyName asSetterMethodName]; \
        objc_msgSend(self,sel_getUid([propertySetter UTF8String]), [NSDecimalNumber numberWithDouble: value ]) ; \
    }



#define AR_ROLLBACK_TO(status)   do { [status rollback] ; return; } while(0)
#define ar_rollback_to(status)   do { [status rollback] ; return; } while(0)

@class ARTransactionState;

typedef void (^ARSavePointTransactionBlock)(ARTransactionState *state);

@interface ARTransactionState : NSObject {}
- (void) rollback;
@property (nonatomic, readonly) BOOL isRolledBack;
@property (nonatomic, readonly) NSString *name;
- (instancetype)initWithName:(NSString *)aName;
+ (instancetype)stateWithName:(NSString *)aName;
@end

@interface NSString (ARAdditions)
- (NSString *) asSetterMethodName;
@end

@interface ActiveRecord (Extensions)
+ (instancetype) findById: (id) record_id;
+ (instancetype) findByKey: (id) key value: (id) value;
+ (instancetype) findOrBuildByKey: (id) key value: (id) value;
+ (NSArray *) findAllByKey: (id) key value: (id) value;
+ (NSArray *) findAllByConditions: (NSDictionary *) conditions;
+ (instancetype) findByConditions: (NSDictionary *) conditions;
+ (void)addSearchOn:(NSString *)aField;
+ (BOOL) savePointTransaction: (ARSavePointTransactionBlock) transaction;
+ (BOOL) savePoint: (NSString *)name transaction: (ARSavePointTransactionBlock) transaction;
- (instancetype) recordSaved;
@end
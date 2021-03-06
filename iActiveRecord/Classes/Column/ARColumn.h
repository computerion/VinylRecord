//
//  ARColumn.h
//  iActiveRecord
//
//  Created by Alex Denisov on 29.04.12.
//  Copyright (c) 2012 okolodev.org. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <objc/runtime.h>
#import "ARColumnType.h"

@class ActiveRecord;

@interface ARColumn : NSObject
@property (nonatomic, copy, readonly) NSString *mappingName;
@property (nonatomic, copy, readonly) NSString *columnName;
@property (nonatomic, strong, readonly) Class columnClass;
@property (nonatomic, strong, readonly) Class recordClass;
@property (nonatomic, copy, readonly) NSString *getter;
@property (nonatomic, copy, readonly) NSString *setter;
@property (nonatomic, readwrite, getter = isDynamic) BOOL dynamic;
@property (nonatomic, readonly) ARColumnType columnType;
@property (nonatomic, readonly, getter=isNullable) BOOL nullable;
@property (nonatomic, readonly, getter=isImmutable) BOOL immutable;
@property (nonatomic, readwrite) objc_AssociationPolicy associationPolicy;

- (instancetype)initWithProperty:(objc_property_t)property ofClass:(Class)aClass;
- (instancetype)initWithProperty:(objc_property_t)property mapping:(NSDictionary*) mapping ofClass:(Class)aClass;
- (NSString *)sqlValueForRecord:(ActiveRecord *)aRecord;
- (id) deserializedValue: (id) value; //todo: rename
- (const char *)sqlType;
- (const char *)columnKey;
@end

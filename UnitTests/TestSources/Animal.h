//
//  Animal.h
//  iActiveRecord
//
//  Created by Alex Denisov on 31.03.12.
//  Copyright (c) 2012 okolodev.org. All rights reserved.
//

#import "ActiveRecord.h"
#import "ARSynchronizationProtocol.h"

@interface Animal : ActiveRecord     <ARSynchronizationProtocol>

@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSString *state;
@property (nonatomic, copy) NSString *title;





@end

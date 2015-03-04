# VinylRecord
VinylRecord (short for Vinyl ActiveRecord) is an ActiveRecord ORM for iOS using SQLite without CoreData. It was fork of a discontinued project, iActiveRecord by Alex Denisov which was a great start for an ORM when they were few others in site. Desiring rapid fixes and new features for an application in dire need of a persistence layer, the API was forked with new name as to avoid confusion with its predecessor, for which it maintains compatibility, and other ORM's with the ActiveRecord name.

# FEATURES
* Belongs-To, Has-Many and Has-Many-Through relationships    
* Nested SavePointTransactions (NEW)
* Thread Safe Transactions (NEW)
* Save, Update, and Syncing of Models
* Traditional and Custom Validations
* Save/Update Callbacks (NEW)

# COMING SOON
* Official Swift Support (currently works best via subclass)

# INSTALLATION  
## Using CocoaPods (Recommend)
***
    # Podfile
    pod 'VinylRecord', '1.0.3'
    
## Using Source
***

``` 
    git clone https://github.com/valerius/VinylRecord.git
```
1. Checkout code
2. Open with XCode and build Build_Framework target.  
3. The ActiveRecord.framework will be located in ~/Products/, this path can be changed in the shell-script in the Settings tab of target.  
4. Open your project Settings and add ActiveRecord.framework and sqlite3.0.dylib to Link Binary With Libraries section.  



# USAGE
## Initialize the database

Upon your application loading you should initialize the database. Keep in mind, that a configuration only happens once, so if you use CocoaPods which has VinylRecord as a dependency, you should initialize your database before utilizing the CocoaPods library initialization procedures, if any, otherwise your settings will not have an effect.

```objective-c
    - (BOOL)application:(UIApplication *)application  didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
    {
        [VinylRecord applyConfiguration:^(ARConfiguration *config) {
                        config.databasePath = ARCachesDatabasePath(nil); //creates default database path
                        config.enableThreadPool = YES; //Enables a database connection per thread
                        config.migrationsEnabled = YES; //Enables simple migrations
        }];
    }
```

## Create your models
To create a model you should subclass the VinylRecord class, which is a descendant of its predecessors ActiveRecord class which it remains compatible with. The Framework will automatically create a database table using the classes inherited from VinylRecord when you run app first time.

So based on this code
```objective-c

    #import <ActiveRecord/VinylRecord.h> // when not using CocoaPods
    #import "VinylRecord.h" // when using CocoaPods
    
    @interface User : VinylRecord
        column_dec(string,name)
        column_dec(boolean,active)
         // or  @property (nonatomic, retain) NSString *name;
    @end
    
    @implementation User
        column_imp(string,name) // or @dynamic 
        column_imp(boolean,active) // user.is_active = YES or user.has_active not available using @dynamic
    @end
``` 

Table 'user' will be created. It will contain 'name' and 'active' field.  

After describing the model you can start using the framework for persisting and querying objects:

```objective-c
    User *user = [User new];
    user.name = @"Alex";
    [user save];

    NSArray *users = [User allRecords];
    User *userForRemove = [users first];
    [userForRemove dropRecord];
```
This code creates, retrieves a list and deletes the model.

## Excluding certain properties

What about the properties that should not be stored in a database?  Currently all properties that are not marked @dynamic are ignored.

# DATA TYPES
## Definitions
VinylRecord supports the typical sql types, which may be either defined using simple macro definition (recommended), or using standard property definitions with the respective @dynamic attribute in the implementation. The macros are prefered because they easily help you visually distinguish between database columns and in some cases, provides additional helper methods.

```objective-c

    @interface DataTypes : VinylRecord
        column_dec(string,property_of_string) // NSString
        column_dec(integer,property_of_integer) // NSNumber
        column_dec(boolean,property_of_boolean) // NSNumber
        column_dec(decimal,property_of_decimal) // NSDecimalNumber
        column_dec(blob,property_of_blob) // NSData
        column_dec(date,property_of_date) // NSDate
    @end
    
    @implementation DataTypes
        column_imp(string,property_of_string)
        column_imp(integer,property_of_integer) // dataTypes.int_property_of_integer = 10; or dataTypes.int_property_of_integer = @(10)
        column_imp(boolean,property_of_bool) // dataTypes.has_property_of_bool,is_property_of_bool or [dataTypes.property_of_bool booleanValue]
        column_imp(decimal,property_of_decimal)
        column_imp(blob,property_of_blob)
        column_imp(date,property_of_date)
        // or @dynamic property_name which doesn't generate helpers
    @end
    
```

## Custom Data Types

All custom property types should implement ARRepresentationProtocol:

```objective-c
    @protocol ARRepresentationProtocol

    @required
    + (const char *)sqlType;
    + (id)fromSql:(NSString *)sqlData;
     - (NSString *)toSql;
    @end
```

This is a simple example:

```objective-c
    @implementation NSString (sqlRepresentation)
    + (ActiveRecord *)fromSql:(NSString *)sqlData{
        return sqlData;
    }

    - (NSString *)toSql {
        return self;
    }

    + (const char *)sqlType {
        return "text";
    }
    @end
```



# VALIDATION

Validations may be registered using the validation_do helper macro inside the implementation of the model.


```objective-c
    // User.h
    @interface User : VinylRecord  <ARValidatorProtocol>
        column_dec(string,name)
    @end
    
    // User.m
    @implementation User
        column_imp(string,name)
        
        validation_do(
            validate_uniqueness_of(name)
            validate_presence_of(name)
        )
    @end
```

 Standard validations, such as validate_uniqueness_of and validate_presence_of will have existing helpers, but you may add your own custom validations also. Just define a new class and implement <ARValidatorProtocol> as follows:
 

```objective-c
   
    //  Custom validator
    //  PrefixValidator.h
    @interface PrefixValidator : NSObject <ARValidatorProtocol>
    @end
    
    //  PrefixValidator.m
    @implementation PrefixValidator
    - (NSString *)errorMessage {
        return @"Invalid prefix";
    }
    - (BOOL)validateField:(NSString *)aField ofRecord:(id)aRecord {
        NSString *aValue = [aRecord valueForKey:aField];
        BOOL valid = [aValue hasPrefix:@"LOL"];
        return valid;
    }
    @end
```

then add validator to your ActiveRecord implementation

    @implementation User
    @synthesize name;
    ...
    validation_do(
        validate_field_with_validator(name, PrefixValidator)
    )
    ...
    @end

Validation errors

    User *user = [User new];
    if(![user isValid]){
        NSArray *errors = [user errorMessages];
        //  do something
    }

or

    User *user = [User new];
    if(![user save]){
        NSArray *errors = [user errorMessages];
        //  do something
    }

# MIGRATIONS  

VinylRecord supports simple migrations which will automatically add new tables or new columns to an already existing table, without loss of data. if you don't need migrations, you should disable them at start of application.

```objective-c
    - (BOOL)application:(UIApplication *)application  didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
    {
        [VinylRecord applyConfiguration:^(ARConfiguration *config) {
                        config.migrationsEnabled = NO;
        }];
    }
```

# TRANSACTIONS

VinylRecords supports regular and save point transactions which are now thread safe, utilizing  SQLite support for Isolation and Concurrency by maintaining a separate database connection for each thread accessing the database which are automatically shutdown when the thread exits and separately, serially queued using GCD. This ensures that transaction in a background thread if rolled back, doesn't rollback unrelated queries placed on a separate thread. You may also nest SAVEPOINT transactions as deep as SQLite and your phone will allow.

```objective-c   

    [VinylRecord transaction:^{
        User *alex = [User new];
        alex.name = @"Alex";
        [alex save];
    }];

    [VinylRecord transaction:^{
        User *alex = [User new];
        alex.name = @"Alex";
        [alex save];
        ar_rollback;
    }];
    
    [VinylRecord savePointTransaction:^(ARTransactionState *state_a) {
        ... // A Changes.
        BOOL success = [ActiveRecord savePointTransaction:^(ARTransactionState *state_b) {
            ... // These changes are folled back
            ar_rollback_to(state_b);
    
        }]; 
                   
        if(success) {
            //we could rollback, or keep state_a change. To rollback: ar_rollback(state_a);        
        }
    }]; 
```
# RELATIONSHIPS

Relationships support "ON DELETE" dependencies: ARDependencyDestroy and ARDependencyNullify

## HasMany <-> BelongsTo
```objective-c 
    // User.h
    @interface User : VinylRecord
    ...
    @property (nonatomic, retain) NSNumber *groupId;
    ...
    belongs_to_dec(Group, group, ARDependencyDestroy)
    ...
    @end
    // User.m
    @implementation User
    ...
    @synthesize groupId;
    ...
    belongs_to_imp(Group, group, ARDependencyDestroy)
    ...
    @end
```
belongs_to_dec and belongs_to_imp take two parameters: the model name and accessor's name.  

The main thing to remember when describe the field of relationship, it must match the name of the model and begin with lowercased letters.

Group      <->     groupId  
User       <->     userId  
ContentManager <->  contentManagerId  

## Describe reverse relationship
```objective-c 
    // Group.h
    @interface Group : VinylRecord
    ...
    has_many_dec(User, users, ARDependencyDestroy)
    ...
    @end
    // Group.m
    @implementation Group
    ...
    has_many_imp(User, users, ARDependencyDestroy)
    ...
    @end
```
The same as above: model name and accessor.  

In order to add or remove model you can use methods that match this pattern:  

    add##ModelName:(ActiveRecord *)aRecord;
    remove##ModelName:(ActiveRecord *)aRecord;

At the moment it works correctly only if both models are saved, otherwise they have no id and relationship is not possible.

## HasManyThrough

To describe this relationship, you need to create another model
    
    // User.h
    @interface User : VinylRecord
    ...
    has_many_through_dec(Project, UserProjectRelationship, projects, ARDependencyNullify)
    ...
    @end
    // User.m
    @implementation User
    ...
    has_many_through_imp(Project, UserProjectRelationship, projects, ARDependencyNullify)
    ...
    @end
    
    // Project.h
    @interface Project : VinylRecord
    ...
    has_many_through_dec(User, UserProjectRelationship, users, ARDependencyDestroy)
    ...
    @end
    // Project.m
    @implementation Project
    ...
    has_many_through_imp(User, UserProjectRelationship, users, ARDependencyDestroy)
    ...
    @end

Relationship model

```objective-c

    // UserProjectRelationship.h
    @interface UserProjectRelationship : VinylRecord
    @property (nonatomic, retain) NSNumber *userId;
    @property (nonatomic, retain) NSNumber *projectId;
    @end
    // UserProjectRelationship.m
    @implementation UserProjectRelationship
    @synthesize userId;
    @synthesize projectId;
    @end
    
```

HasManyThrough has the same gaps as HasMany.

# Requests and filters

The framework allows to obtain a list of records using different filters, all requests are lazy, i.e., being run only on demand.
A simple example of obtaining a limited number of records:  

    NSArray *users = [[[User query] limit:5] fetchRecords];

All filters returns an instance of ARLazyFetcher, which allows you to write queries in a single row:

    NSArray *users = [[[[User query] offset:5] limit:2] fetchRecords];

The fetchRecords method initiates sql-query to the database on the basis of all applied filters:

    ARLazyFetcher *fetcher = [[[User query] offset:2] limit:10];
    [fetcher whereField:@"name"
           equalToValue:@"Alex"];
    NSArray *users = [fetcher fetchRecords];

There are several options for 'where' methods:
    - (ARLazyFetcher *)where:(NSString *)aFormat, ...;


You can also concatenate with finalStatement some "complex" or a simple filter. 

Relationship HasManyThrough is already use the 'where' filter, so if you want to add more, you should use code like this:

    NSArray *ids = [NSArray arrayWithObjects:
                   [NSNumber numberWithInt:1],
                   [NSNumber numberWithInt:15], 
                   nil];
                  
    ARLazyFetcher *fetcher = [[User query]  where:@"'name' = %@ or 'id' in %@",  @"john", ids, nil];
    NSArray *records = [[fetcher orderBy:@"id"] fetchRecords];

New syntax support basic comparisons:

 - =, == - equality
 - >= - left-hand operand greater or equal then right-hand operand
 - <= - right-hand operand greater or equal then left-hand operand
 - > - left-hand operand greater then right-hand operand
 - < - right-hand operand greater then left-hand operand
 - !=, <> - operands are not equal
 - LIKE, NOT LIKE - pattern matching
 - IN, NOT IN container - records which fields are in container (NSArray, NSSet etc.)
 - BETWEEN %1 AND %2 - records with fields in range(%1, %2)

# Fetch only needed fields

    - (ARLazyFetcher *)only:(NSString *)aFirstParam, ...;
    - (ARLazyFetcher *)except:(NSString *)aFirstParam, ...;

    ARLazyFetcher *fetcher = [[User query] only:@"name", @"id", nil];
    NSArray *users = [fetcher fetchRecords];

# Sorting

    - (ARLazyFetcher *)orderBy:(NSString *)aField ascending:(BOOL)isAscending;
    - (ARLazyFetcher *)orderBy:(NSString *)aField;// ASC by default

    ARLazyFetcher *fetcher = [[[User query] offset:2] limit:10];
    [[fetcher whereField:@"name"
           equalToValue:@"Alex"] orderBy:@"name"];
    NSArray *users = [fetcher fetchRecords];

# Join

The framework supports the use of joins:

    - (ARLazyFetcher *)join:(Class)aJoinRecord
                    useJoin:(ARJoinType)aJoinType
                    onField:(NSString *)aFirstField
                   andField:(NSString *)aSecondField;

aJoinRecord - a model for which we use join  
aJoinType - type of join,  may be several options  
* ARJoinLeft  
* ARJoinRight  
* ARJoinInner  
* ARJoinOuter  
aFirstField - a field on which the current model will be join  
aSecondField - a field on which the aJoinModel will be join  

Joined records can be fetched by sending

    - (NSArray *)fetchJoinedRecords;

message to ARLazyFetcher instance.


from pymongo import MongoClient
from bson.objectid import ObjectId

# To import for testing, move into same directory as notebook and use import statement as seen above
# Pass key-value pair to functions and check value of return...

# Modify for "generic" database use? Designate database, port, username, etc. when object is instantiated?

class AnimalShelter(object):
    """ CRUD operations for Animal collection in MongoDB """
    
    def __init__(self):
        # Initializing the MongoClient. This helps to 
        # access the MongoDB databases and collections.
        # This is hard-wired to use the aac database, the 
        # animals collection, and the aac user.
        # Definitions of the connection string variables are
        # unique to the individual Apporto environment.
        #
        # You must edit the connection variables below to reflect
        # your own instance of MongoDB!
        #
        # Connection Variables
        #
        USER = 'aacuser'
        PASS = 'SNHU1234'
        HOST = 'nv-desktop-services.apporto.com'
        # PORT = 31580
        PORT = 32839
        # DB = 'aac'
        DB = 'AAC'
        COL = 'animals'
        #
        # Initialize Connection
        #
        self.client = MongoClient('mongodb://%s:%s@%s:%d' % (USER,PASS,HOST,PORT))
        self.database = self.client['%s' % (DB)]
        self.collection = self.database['%s' % (COL)]

    def create(self, data):
        
        # Boolean to track and return success
        insertSuccess = False
        
        # Insert document into specified collection
        if (data is not None):
            insertSuccess = self.database.animals.insert_one(data).acknowledged
        else:
            raise Exception("Nothing to save, because data parameter is empty")
        
        # Return bool indicating whether insertion was successful
        return insertSuccess
                
        
    def read(self, data):
        
        # List of results to be returned; will be empty if read is unsuccessful
        results = list()
        
        # Look for document in specified collection
        if (data is not None):
            results = list(self.database.animals.find(data))
        else:
            raise Exception("Invalid query")
            
        # Return bool indicating whether read operation was successful
        return results
    
    
    def update(self, data, updData):
        numUpd = 0
        
        if (data is not None):
            numUpd = self.database.animals.update_many(
                data, {"$set": updData}).modified_count
        else:
            raise Exception("Nothing to update")
            
        return numUpd
     
        
       
    def delete(self, data):
        numDel = 0
        
        if (data is not None):
            numDel = self.database.animals.delete_many(data).deleted_count
        else:
            raise Exception("Nothing to delete")
            
        return numDel
      
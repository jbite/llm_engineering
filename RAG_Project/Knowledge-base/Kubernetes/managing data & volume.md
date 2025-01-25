Volumes
Persistent Volumes & Persistent Volume Claims
Environement Vairables

Local volumes Vs Cloud volumes

deployment

apiVersion: apps/v1
kind: Deployment
metadata:
  name: story-deployment
spec:
  replicas: 1
  selector:
    matchLabels:
      app: story
  template:
    metadata:
      labels:
        app: story
    spec:
      containers:
        - name: story
          image: jbite9057/kub-data-demo:1
          resources:
            limits:
              cpu: "100m"
              memory: "50Mi"
          volumeMounts:
           - mountPath: /app/story
		name: story-volume
      volumes:
        - name: story-volume
          emptyDir: {}

if we have over 2 replicas, the connection will need to auto distribute to pods by hostPath volume type
change volume as follow
volumes:
        - name: story-volume
          hostPath:
            path: /data
            type: DirectoryOrCreate

CSI type

container storage interface
like AWS elastic block storage
with CSI, we can easily to add AWS EFS as kubernetes volume


Persist volume

volumes are destroyed when a pod is removed
hostPath partially works around that in One-node environments
Pod and Node-independent volumes are sometimes required
that is persistent volumes run into play

use PV claim to connect with persistent volume in a node
can also connect to multiple persistent volumes

we need to define volume first for container to claim
host-pv.yaml
apiVersion: v1
kind: PersistentVolume
metadata:
  name: host-pv
spec:
  capacity:
    storage: 128Mi
  volumeMode: Block
  accessModes:
    - ReadWriteOnce
    - ReadOnlyMany
    - ReadWriteMany
  hostPath:
    path: /data
    type: DirectoryOrCreate


Persistent Volumes | Kubernetes

Once volumes defined, volume could be claimed by container
host-pvc.yaml
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: host-pvc
spec:
  volumeName: host-pv
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 128Mi


change config in deployment.yaml

 volumes:
        - name: story-volume
          persistentVolumeClaim:
            claimNam: host-pvc

Understanding “State”
State is data created and used by your application which must not be lost

User-generated data, user accounts,...: often stored in a database, but could also be files (e.g. uploads)
Intermediate results derived by the app: often stored in memory, temporary database tables or files

Volumes allow you to persist data
Normal Volumes: volumes is attached to Pod and Pod lifecycle, defined and created together with Pod, Repetitive and hard to administer on a global level
Persistent Volumes: 
Volume is a standalone Cluster resource 
Created standalone, claimed via a PVC
Can be defined once and used multiple times

